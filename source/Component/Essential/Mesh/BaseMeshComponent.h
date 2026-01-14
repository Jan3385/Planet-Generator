#pragma once

#include <vector>
#include "Component/BaseComponent.h"

namespace Component {
class BaseMesh : public BaseComponent {
public:
    struct IMeshUpdateCallback{
    protected:
        virtual void OnMeshUpdated(BaseMesh* mesh) = 0;
        virtual ~IMeshUpdateCallback() = default;

        friend class BaseMesh;
    };

    BaseMesh(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~BaseMesh() override;

    virtual std::vector<float> GetVerticies() const = 0;

    virtual bool IsEmpty() const { return true; }

    void AddUpdateCallback(IMeshUpdateCallback* callback) {
        this->updateCallbacks.push_back(callback);
    }
    void RemoveUpdateCallback(IMeshUpdateCallback* callback) {
        std::erase(updateCallbacks, callback);
    }
protected:
    void NotifyMeshUpdated() {
        for(auto& callback : updateCallbacks){
            callback->OnMeshUpdated(this);
        }
    }

    virtual std::vector<std::type_index> GetDependencies() const override 
        { return {}; }
private:
    std::vector<IMeshUpdateCallback*> updateCallbacks;
};
}