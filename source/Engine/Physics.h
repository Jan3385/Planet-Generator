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