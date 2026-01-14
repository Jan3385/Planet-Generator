#pragma once

#include <vector>

#include "Component/BaseComponent.h"
#include "GLWrapper/Mesh.h"

namespace Component {
class Mesh : public BaseComponent {
public:
    struct IMeshUpdateCallback{
    protected:
        virtual void OnMeshUpdated(Mesh* mesh) = 0;
        virtual ~IMeshUpdateCallback() = default;

        friend class Mesh;
    };

    Mesh(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~Mesh() override;
    void SetMeshData(std::vector<float> verticies, std::vector<float> normals);

    std::vector<float> GetVerticies() const { return verticies; }
    std::vector<float> GetNormals() const { return normals; }

    bool IsEmpty() const { return verticies.empty(); }

    void AddUpdateCallback(IMeshUpdateCallback* callback) {
        this->updateCallbacks.push_back(callback);
    }
    void RemoveUpdateCallback(IMeshUpdateCallback* callback) {
        std::erase(updateCallbacks, callback);
    }
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {}; }
private:
    std::vector<float> verticies;
    std::vector<float> normals;
    std::vector<IMeshUpdateCallback*> updateCallbacks;
};
}