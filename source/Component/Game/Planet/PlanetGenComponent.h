#pragma once

#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"
#include "Component/Engine/Renderer/MeshRenderComponent.h"
#include "Component/Engine/Physics/ConvexHullColliderComponent.h"
#include "Math/Color.h"

namespace Component {
/**
 * 
 */
class PlanetGen : public BaseComponent, public Component::IUpdatable, public Component::IImGuiUpdatable{
public:
    static constexpr float PLANET_SCALE = 1.0f;

    struct alignas(16) MaterialSTD140{
        glm::vec4 color;
        glm::vec4 MetalRought; // .r = metallic, .g = roughness
        MaterialSTD140(glm::vec3 color, glm::vec2 MetalRought)
            : color(color, 1), MetalRought(MetalRought.x, MetalRought.y, 0, 0) {};
    };

    PlanetGen(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~PlanetGen() override = default;

    void PlanetifyMesh(uint32_t seed);

    void SetMaterials(std::string planetMatName, std::string atmosphereMatName);
    void SetAtmosphereColors(const glm::vec4& horizonColor, const glm::vec4& zenithColor);
    void SetPlanetColors(
        const MaterialSTD140& deepOcean, 
        const MaterialSTD140& shallowOcean, 
        const MaterialSTD140& sand, 
        const MaterialSTD140& grass, 
        const MaterialSTD140& rock, 
        const MaterialSTD140& snow
    );
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform) }; }

    void ImGuiUpdate() override;
private:
    struct alignas(16) atmospherePalette{
        glm::vec4 horizonColor;
        glm::vec4 zenithColor;
    };
    struct alignas(16) planetPalette{
        MaterialSTD140 deepOcean;
        MaterialSTD140 shallowOcean;
        MaterialSTD140 sand;
        MaterialSTD140 grass;
        MaterialSTD140 rock;
        MaterialSTD140 snow;
    };

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void Update() override;
    void FixedUpdate() override;

    Transform* transform = nullptr;
    MeshRender* planetRenderComponent = nullptr;
    MeshRender* atmosphereRenderComponent = nullptr;
    ConvexHullCollider* planetCollider = nullptr;
    

    std::shared_ptr<GL::Mesh> planetMesh;
    std::shared_ptr<GL::Mesh> atmosphereMesh;
    GL::Buffer<atmospherePalette, GL::BufferTarget::UniformBuffer> atmospherePaletteBuffer;
    GL::Buffer<planetPalette, GL::BufferTarget::UniformBuffer> planetPaletteBuffer;

    bool rotatePlanet = true;

    friend class Object::BaseObject;
};
}