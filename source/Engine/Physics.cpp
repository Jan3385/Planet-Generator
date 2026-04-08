#include "Physics.h"

#include "Engine/Engine.h"

#include <cstdarg>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "Debug/Logger.h"

#include "Component/Engine/Physics/BaseColliderComponent.h"

Physics* Physics::instance = nullptr;

// how many substeps happen each physics update
constexpr uint8_t collisionSteps = 1;

constexpr uint32_t allocatorSize = 10 * 1024 * 1024; // 10 MB

// Broadphase layer interface
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::uint NUM_LAYERS = 2;
class _BPLayerInterface : public JPH::BroadPhaseLayerInterface {
public:
    _BPLayerInterface() = default;

    virtual JPH::uint GetNumBroadPhaseLayers() const override {
        return static_cast<JPH::uint>(NUM_LAYERS);
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        switch (static_cast<Physics::Layer>(inLayer))
        {
        case Physics::Layer::Static:
        case Physics::Layer::NO_COLLISION:
        case Physics::Layer::Terrain:
            return NON_MOVING;
        case Physics::Layer::Kinematic:
            return MOVING;
        case Physics::Layer::Dynamic:
            return MOVING;
        default:
            Debug::LogFatal("[JPH] Invalid object layer: " + std::to_string(inLayer));
            return NON_MOVING;
        }
    }
};

// Object vs broadphase layer filter
class _ObjectVsBPLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    _ObjectVsBPLayerFilterImpl() = default;

    virtual bool ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer bpLayer) const override {
        switch (static_cast<Physics::Layer>(layer)) {
            case Physics::Layer::NO_COLLISION:
                return false;               // NO_COLLISION layer does not collide with anything
            case Physics::Layer::Static:
            case Physics::Layer::Terrain:
                return bpLayer == MOVING;   // static only collides with moving
            case Physics::Layer::Kinematic:
                return true;                // kinematic collides with everything
            case Physics::Layer::Dynamic:
                return true;                // dynamic collides with everything

            default:
                Debug::LogFatal("[JPH] Invalid object layer: " + std::to_string(layer));
                return false;
        }
    }
};

// Object layer pair filter
class _ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
public:
    _ObjectLayerPairFilter() = default;

    virtual bool ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const override {
        switch (static_cast<Physics::Layer>(layer1)) {
            case Physics::Layer::NO_COLLISION:
                return false;                                                           // NO_COLLISION layer does not collide with anything
            case Physics::Layer::Static:
            case Physics::Layer::Terrain:
                return static_cast<Physics::Layer>(layer2) == Physics::Layer::Dynamic;  // static only collides with dynamic
            case Physics::Layer::Kinematic:
                return true;                                                            // kinematic collides with everything
            case Physics::Layer::Dynamic:
                return true;                                                            // dynamic collides with everything
            default:
                Debug::LogFatal("[JPH] Invalid object layer: " + std::to_string(layer1));
                return false;

        }
    }
};

static void JoltTraceImpl(const char* inFMT, ...) {
    va_list args;
    va_start(args, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, args);
    va_end(args);
    Debug::LogTrace(std::string("[JPH] ") + buffer);
}

static bool JoltAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine) {
    Debug::LogError(
        std::string("[JPH Assert] ") + inExpression +
        " | " + (inMessage ? inMessage : "") +
        " | " + inFile + ":" + std::to_string(inLine)
    );
    return true;
}

Physics::Physics()
{
    if(JPH::Factory::sInstance != nullptr) {
        Debug::LogFatal("[JPH] Factory instance already exists. Creating multiple physics instances is not supported!");
    }

    JPH::Trace = JoltTraceImpl;
    JPH::AssertFailed = JoltAssertFailed;

    // Initialize Jolt Physics
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    // Setup allocator and job system
    tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(allocatorSize);
    jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs, 
        JPH::cMaxPhysicsBarriers, 
        -1
    );

    // Layers
    this->bpLayerInterface = std::make_unique<_BPLayerInterface>();
    this->objectVsBpLayerFilter = std::make_unique<_ObjectVsBPLayerFilterImpl>();
    this->objectLayerPairFilter = std::make_unique<_ObjectLayerPairFilter>();

    this->contactListener = std::make_unique<PhysicsContactListener>();

    // Create physics system
    this->physicsSystem = std::make_unique<JPH::PhysicsSystem>();
    this->physicsSystem->Init(
        1024,   // max bodies
        0,      // auto detect body mutexes
        1024,   // max body pairs
        1024,   // max contact constraints
        *this->bpLayerInterface,
        *this->objectVsBpLayerFilter,
        *this->objectLayerPairFilter
    );

    this->physicsSystem->SetContactListener(this->contactListener.get());

    physicsSystem->OptimizeBroadPhase();

    physicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
}

Physics::~Physics()
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    if(this->instance == this) this->instance = nullptr;
}

void Physics::Update(float deltaTime)
{
    this->physicsSystem->Update(
        deltaTime,
        collisionSteps,
        this->tempAllocator.get(),
        this->jobSystem.get()
    );

    while (!this->contactListener->eventQueue.empty()) {
        auto event = this->contactListener->eventQueue.front();
        this->contactListener->eventQueue.pop();

        Component::BaseCollider* col1 = this->bodyIDToColliderMap[event.id1];
        Component::BaseCollider* col2 = this->bodyIDToColliderMap[event.id2];

        switch (event.type)
        {
        case PhysicsContactListener::ContactEvent::Type::Added:
            if(col1 && col1->onCollisionEnter) {
                col1->onCollisionEnter({
                    .otherID = event.id2,
                    .otherCollider = col2,
                    .manifold = event.manifold
                });
            }
            if(col2 && col2->onCollisionEnter) {
                col2->onCollisionEnter({
                    .otherID = event.id1,
                    .otherCollider = col1,
                    .manifold = event.manifold.SwapShapes()
                });
            }
            break;
        case PhysicsContactListener::ContactEvent::Type::Persisted:
            if(col1 && col1->onCollisionStay) {
                col1->onCollisionStay({
                    .otherID = event.id2,
                    .otherCollider = col2,
                    .manifold = event.manifold
                });
            }
            if(col2 && col2->onCollisionStay) {
                col2->onCollisionStay({
                    .otherID = event.id1,
                    .otherCollider = col1,
                    .manifold = event.manifold.SwapShapes()
                });
            }
            break;
        case PhysicsContactListener::ContactEvent::Type::Removed:
            if(col1 && col1->onCollisionExit) {
                col1->onCollisionExit(event.id2);
            }
            if(col2 && col2->onCollisionExit) {
                col2->onCollisionExit(event.id1);
            }
            break;
        default:
            Debug::LogWarn("[JPH] Unknown contact event type: " + std::to_string(static_cast<int>(event.type)));
            break;
        }
    }

}

void Physics::SetGlobalGravity(const glm::vec3 &gravity)
{
    physicsSystem->SetGravity(JPH::Vec3(gravity.x, gravity.y, gravity.z));
}

/// @brief Creates a physics body for the physics simulation
/// @param settings The settings for the body creation, including shape, mass, friction, etc.
/// @param caller The collider component that called CreateBody, can be nullptr
/// @return The ID of the created body
JPH::BodyID Physics::CreateBody(const JPH::BodyCreationSettings &settings, Component::BaseCollider* caller)
{
    JPH::BodyInterface& bodyInterface = this->physicsSystem->GetBodyInterface();

    Debug::AssertNot(settings.GetShape() == nullptr, "Cannot create a body with a null shape!");

    JPH::EActivation activationMode = 
        settings.mMotionType == JPH::EMotionType::Static ? JPH::EActivation::DontActivate : JPH::EActivation::Activate;

    JPH::BodyID body = bodyInterface.CreateAndAddBody(
        settings,
        activationMode
    );

    Debug::AssertNot(body.IsInvalid(), "Failed to create body!");

    this->bodyIDToColliderMap[body] = caller;

    return body;
}

void Physics::EnableBody(JPH::BodyID bodyID)
{
    if(bodyID.IsInvalid()) [[unlikely]] {
        Debug::LogWarn("[JPH] Attempted to enable an invalid body ID");
        return;
    }

    this->physicsSystem->GetBodyInterface().ActivateBody(bodyID);
}

void Physics::DisableBody(JPH::BodyID bodyID)
{
    if(bodyID.IsInvalid()) [[unlikely]] {
        Debug::LogWarn("[JPH] Attempted to disable an invalid body ID");
        return;
    }

    this->physicsSystem->GetBodyInterface().DeactivateBody(bodyID);
}

void Physics::RemoveBody(JPH::BodyID &bodyID)
{
    if(bodyID.IsInvalid()) [[unlikely]] {
        Debug::LogWarn("[JPH] Attempted to remove an invalid body ID");
        return;
    }
    
    this->bodyIDToColliderMap.erase(bodyID);
    this->physicsSystem->GetBodyInterface().RemoveBody(bodyID);
    this->physicsSystem->GetBodyInterface().DestroyBody(bodyID);
}

void Physics::SetLayer(JPH::BodyID bodyID, Layer layer)
{
    if(bodyID.IsInvalid()) [[unlikely]] {
        Debug::LogWarn("[JPH] Attempted to set layer of an invalid body ID");
        return;
    }

    this->physicsSystem->GetBodyInterface().SetObjectLayer(bodyID, static_cast<JPH::ObjectLayer>(layer));
}

void Physics::UpdateBodyTransform(JPH::BodyID bodyID, const JPH::RVec3 &position, const JPH::Quat &rotation, bool kinematic)
{
    if(!kinematic)
        this->physicsSystem->GetBodyInterface().SetPositionAndRotation(bodyID, position, rotation, JPH::EActivation::DontActivate);
    else
        this->physicsSystem->GetBodyInterface().MoveKinematic(bodyID, position, rotation, GameEngine::instance->DeltaTime());
}

glm::vec3 Physics::GetBodyPosition(JPH::BodyID bodyID)
{
    const JPH::RVec3 pos = this->physicsSystem->GetBodyInterface().GetPosition(bodyID);

    return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
}

glm::quat Physics::GetBodyRotation(JPH::BodyID bodyID)
{
    const JPH::Quat rot = this->physicsSystem->GetBodyInterface().GetRotation(bodyID);

    return glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
}

JPH::EMotionType Physics::GetMotionType(Layer layer)
{
    switch (layer)
    {
        case Layer::Static:
        case Layer::Terrain:
            return JPH::EMotionType::Static;
        case Layer::Kinematic:
            return JPH::EMotionType::Kinematic;
        case Layer::Dynamic:
            return JPH::EMotionType::Dynamic;
        default:
            Debug::LogFatal("[JPH] Invalid object layer: " + std::to_string(static_cast<int>(layer)));
            return JPH::EMotionType::Static;
    }
}

void Physics::PhysicsContactListener::OnContactAdded(
    const JPH::Body &inBody1, const JPH::Body &inBody2, 
    const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    std::lock_guard<std::mutex> lock(this->eventQueueMutex);
    this->eventQueue.push({
        .type = ContactEvent::Type::Added,
        .id1 = inBody1.GetID(),
        .id2 = inBody2.GetID(),
        .manifold = inManifold
    });
}

void Physics::PhysicsContactListener::OnContactPersisted(
    const JPH::Body &inBody1, const JPH::Body &inBody2, 
    const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    std::lock_guard<std::mutex> lock(this->eventQueueMutex);
    this->eventQueue.push({
        .type = ContactEvent::Type::Persisted,
        .id1 = inBody1.GetID(),
        .id2 = inBody2.GetID(),
        .manifold = inManifold
    });
}

void Physics::PhysicsContactListener::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    std::lock_guard<std::mutex> lock(this->eventQueueMutex);

    this->eventQueue.push({
        .type = ContactEvent::Type::Removed,
        .id1 = inSubShapePair.GetBody1ID(),
        .id2 = inSubShapePair.GetBody2ID(),
        .manifold = JPH::ContactManifold()
    });
}
