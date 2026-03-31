#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

class Physics {
public:
    enum class Layer : JPH::ObjectLayer {
        Static = 0,
        Terrain = 1,
        Dynamic = 2,
    };

    Physics();
    ~Physics();

    void Update(float deltaTime);

    JPH::BodyID CreateBody(const JPH::BodyCreationSettings &settings);
    void EnableBody(JPH::BodyID bodyID);
    void DisableBody(JPH::BodyID bodyID);
    void RemoveBody(JPH::BodyID bodyID);

    JPH::BodyInterface& GetBodyInterface() { return this->physicsSystem->GetBodyInterface(); }

    void UpdateBodyTransform(JPH::BodyID bodyID, const JPH::RVec3 &position, const JPH::Quat &rotation);
    glm::vec3 GetBodyPosition(JPH::BodyID bodyID);
    glm::quat GetBodyRotation(JPH::BodyID bodyID);

    static JPH::EMotionType GetMotionType(Layer layer);
private:
    std::unique_ptr<JPH::PhysicsSystem> physicsSystem;

    std::unique_ptr<JPH::TempAllocatorImpl>   tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;

    std::unique_ptr<JPH::BroadPhaseLayerInterface>      bpLayerInterface;
    std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilter> objectVsBpLayerFilter;
    std::unique_ptr<JPH::ObjectLayerPairFilter>         objectLayerPairFilter;

    void* BPLayerInterface   = nullptr;
    void* ObjVsBPLayerFilter = nullptr;
    void* ObjLayerPairFilter = nullptr;
};