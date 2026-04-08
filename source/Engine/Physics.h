#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <queue>

namespace Component {
    class BaseCollider;
}

class Physics {
public:
    enum class Layer : JPH::ObjectLayer {
        Static = 0,
        NO_COLLISION = 1,
        Terrain = 2,
        Kinematic = 3,
        Dynamic = 4,
    };

    class PhysicsContactListener : public JPH::ContactListener {
    public:
        struct ContactEvent{
            enum class Type { Added, Persisted, Removed };
            Type type;
            JPH::BodyID id1, id2;
            JPH::ContactManifold manifold;
        };

        std::queue<ContactEvent> eventQueue;
        
        void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
        void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
        void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;
    private:
        std::mutex eventQueueMutex;
    };

    Physics();
    ~Physics();

    void Update(float deltaTime);

    void SetGlobalGravity(const glm::vec3 &gravity);

    JPH::BodyID CreateBody(const JPH::BodyCreationSettings &settings, Component::BaseCollider* caller);
    void EnableBody(JPH::BodyID bodyID);
    void DisableBody(JPH::BodyID bodyID);
    void RemoveBody(JPH::BodyID &bodyID);
    void SetLayer(JPH::BodyID bodyID, Layer layer);

    JPH::BodyInterface& GetBodyInterface() { return this->physicsSystem->GetBodyInterface(); }

    void UpdateBodyTransform(JPH::BodyID bodyID, const JPH::RVec3 &position, const JPH::Quat &rotation, bool kinematic);
    glm::vec3 GetBodyPosition(JPH::BodyID bodyID);
    glm::quat GetBodyRotation(JPH::BodyID bodyID);

    static JPH::EMotionType GetMotionType(Layer layer);

    Physics* MakeInstance() { instance = this; return this; };

    /// @brief Gets the singleton instance of the Physics class
    static Physics* Ins() { return instance; }
private:
    static Physics* instance;

    std::unique_ptr<JPH::PhysicsSystem> physicsSystem;

    std::unique_ptr<JPH::TempAllocatorImpl>   tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;

    std::unique_ptr<JPH::BroadPhaseLayerInterface>      bpLayerInterface;
    std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilter> objectVsBpLayerFilter;
    std::unique_ptr<JPH::ObjectLayerPairFilter>         objectLayerPairFilter;
    std::unique_ptr<PhysicsContactListener>             contactListener;

    std::unordered_map<JPH::BodyID, Component::BaseCollider*> bodyIDToColliderMap;
};