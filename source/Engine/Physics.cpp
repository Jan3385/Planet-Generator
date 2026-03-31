#include "Physics.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

#include "Debug/Logger.h"

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
        case Physics::Layer::Terrain:
            return NON_MOVING;
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
            case Physics::Layer::Static:
            case Physics::Layer::Terrain:
                return bpLayer == MOVING;   // static only collides with moving
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
            case Physics::Layer::Static:
            case Physics::Layer::Terrain:
                return static_cast<Physics::Layer>(layer2) == Physics::Layer::Dynamic;  // static only collides with dynamic
            case Physics::Layer::Dynamic:
                return true;                                                            // dynamic collides with everything
            default:
                Debug::LogFatal("[JPH] Invalid object layer: " + std::to_string(layer1));
                return false;

        }
    }
};

Physics::Physics()
{
    if(JPH::Factory::sInstance != nullptr) {
        Debug::LogFatal("[JPH] Factory instance already exists. Creating multiple physics instances is not supported!");
    }

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
}

Physics::~Physics()
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void Physics::Update(float deltaTime)
{
    this->physicsSystem->Update(
        deltaTime,
        collisionSteps,
        this->tempAllocator.get(),
        this->jobSystem.get()
    );
}

JPH::BodyID Physics::CreateBody(const JPH::BodyCreationSettings &settings)
{
    JPH::BodyInterface& bodyInterface = this->physicsSystem->GetBodyInterface();

    return bodyInterface.CreateAndAddBody(
        settings,
        JPH::EActivation::Activate
    );
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

void Physics::RemoveBody(JPH::BodyID bodyID)
{
    if(bodyID.IsInvalid()) [[unlikely]] {
        Debug::LogWarn("[JPH] Attempted to remove an invalid body ID");
        return;
    }

    this->physicsSystem->GetBodyInterface().RemoveBody(bodyID);
    this->physicsSystem->GetBodyInterface().DestroyBody(bodyID);
}

void Physics::UpdateBodyTransform(JPH::BodyID bodyID, const JPH::RVec3 &position, const JPH::Quat &rotation)
{
    this->physicsSystem->GetBodyInterface().SetPositionAndRotation(bodyID, position, rotation, JPH::EActivation::DontActivate);
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
        case Layer::Dynamic:
            return JPH::EMotionType::Dynamic;
        default:
            Debug::LogFatal("[JPH] Invalid object layer: " + std::to_string(static_cast<int>(layer)));
            return JPH::EMotionType::Static;
    }
}
