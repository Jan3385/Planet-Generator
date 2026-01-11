#pragma once

namespace Object {
    class BaseObject;
};

namespace Component {
class BaseComponent {
public:
    BaseComponent(Object::BaseObject* owner) : owner(owner) {};
    virtual ~BaseComponent() = default;

    // Disable copy and move
    BaseComponent(const BaseComponent&) = delete;
    BaseComponent& operator=(const BaseComponent&) = delete;
    BaseComponent(BaseComponent&&) = delete;
    BaseComponent& operator=(BaseComponent&&) = delete;

    bool IsAwake() const { return awake; }
    bool IsStarted() const { return started; }

    Object::BaseObject* GetOwner() const { return owner; }
protected:
    virtual void Awake() {}
    virtual void Start() {}
    virtual void OnDestroy() {}
    virtual void OnEnable() {}
    virtual void OnDisable() {}
private:
    Object::BaseObject* owner = nullptr;

    bool awake = false;
    bool started = false;

    friend class Object::BaseObject;
};
class IUpdatable {
protected:
    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual ~IUpdatable() = default;

    friend class Object::BaseObject;
};
}