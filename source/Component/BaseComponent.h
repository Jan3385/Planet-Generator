#pragma once

#include <typeindex>
#include <vector>
#include "Engine/Renderer.h"

#include <imgui.h>

namespace Object {
    class BaseObject;
};

namespace Component {
class BaseComponent {
public:
    BaseComponent(Object::BaseObject* owner) : owner(owner) {};
    virtual ~BaseComponent(){
        this->OnDestroy();
    };

    // Disable copy and move
    BaseComponent(const BaseComponent&) = delete;
    BaseComponent& operator=(const BaseComponent&) = delete;
    BaseComponent(BaseComponent&&) = delete;
    BaseComponent& operator=(BaseComponent&&) = delete;

    bool IsAwake() const { return awake; }
    bool IsStarted() const { return started; }

    void CheckDependencies() const;

    Object::BaseObject* GetOwner() const { return owner; }
protected:
    virtual void Awake() {}
    virtual void Start() {}
    virtual void OnDestroy() {}
    virtual void OnEnable() {}
    virtual void OnDisable() {}

    virtual std::vector<std::type_index> GetDependencies() const { return {}; }
private:
    Object::BaseObject* owner = nullptr;

    bool awake = false;
    bool started = false;

    friend class Object::BaseObject;
};
class IUpdatable {
protected:
    /// @brief Called every frame if the component is enabled
    virtual void Update() = 0;

    /// @brief Called at a fixed time interval if the component is enabled
    /// @note This runs in the same thread as Update, so no concurrency issues arise
    virtual void FixedUpdate() = 0;

    virtual ~IUpdatable() = default;
    friend class Object::BaseObject;
};
class IOffsetUpdatable{
protected:
    /// @brief Called before `Update` if the component is enabled
    virtual void EarlyUpdate() = 0;

    /// @brief Called after `Update` if the component is enabled
    virtual void LateUpdate() = 0;

    virtual ~IOffsetUpdatable() = default;
    friend class Object::BaseObject;
};
class IImGuiUpdatable {
public:
    virtual void ImGuiUpdate() = 0;
protected:
    IImGuiUpdatable();
    ~IImGuiUpdatable();
};
}