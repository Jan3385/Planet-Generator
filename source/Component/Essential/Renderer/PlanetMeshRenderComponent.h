#pragma once

#include "GLWrapper/ColorMesh.h"
#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of a planet
 */
class PlanetMeshRender : public Component::BaseMeshRender{
public:
    struct alignas(16) MaterialSTD140{
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        glm::vec4 shininess; // alignas 16. Only use x component 
        MaterialSTD140(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
            : ambient(ambient, 1), diffuse(diffuse, 1), specular(specular, 1), shininess(shininess, 1, 1, 1) {}
    };
    struct alignas(16) planetPalette{
        MaterialSTD140 deepOcean;
        MaterialSTD140 shallowOcean;
        MaterialSTD140 sand;
        MaterialSTD140 grass;
        MaterialSTD140 rock;
        MaterialSTD140 snow;
    };

    PlanetMeshRender(Object::BaseObject* owner);
    ~PlanetMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;
    void RenderVelocity(GL::Shader &s) override;

    bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override{
        glm::vec3 centroid;
        double radius = this->mesh->GetFrustumRadiusWithCentroid(&centroid, this->transform->GetPos(), this->transform->GetScale());
        return Component::BaseMeshRender::IsInsideFrustum(frustumPlanes, centroid, radius);
    }

    void SetMesh(std::shared_ptr<GL::Mesh> mesh) { this->mesh = mesh; }
    void SetColorPalette(const planetPalette& palette);
    std::shared_ptr<GL::Mesh> GetMesh() const { return this->mesh; }
protected:
    void Awake() override;
    void OnDestroy() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    std::shared_ptr<GL::Mesh> mesh = nullptr;
    GL::Buffer<planetPalette, GL::BufferTarget::UniformBuffer> paletteBuffer;

    friend class Object::BaseObject;
};
}