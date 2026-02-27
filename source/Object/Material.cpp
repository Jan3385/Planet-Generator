#include "Material.h"

Material CommonMaterials[] = {
    { glm::vec3(0.18275f, 0.17f, 0.22525f), 0.0f, 0.85f },
    { glm::vec3(0.61424f, 0.04136f, 0.04136f), 0.0f, 0.65f },
    { glm::vec3(0.780392f, 0.568627f, 0.113725f), 1.0f, 0.60f },
    { glm::vec3(0.714f, 0.4284f, 0.18144f), 1.0f, 0.62f },
    { glm::vec3(0.4f, 0.4f, 0.4f), 1.0f, 0.65f },
    { glm::vec3(0.7038f, 0.27048f, 0.0828f), 1.0f, 0.47f },
    { glm::vec3(0.75164f, 0.60648f, 0.22648f), 1.0f, 0.60f },
    { glm::vec3(0.50754f, 0.50754f, 0.50754f), 1.0f, 0.80f },
    { glm::vec3(0.01f, 0.01f, 0.01f), 0.0f, 0.80f },
    { glm::vec3(0.55f, 0.55f, 0.55f), 1.0f, 0.80f },
    { glm::vec3(0.5f, 0.0f, 0.0f), 0.0f, 0.80f },
    { glm::vec3(0.7f, 0.25f, 0.25f), 0.0f, 0.97f }
};

Material *GetMaterial(MatIndex index)
{
    return &CommonMaterials[static_cast<size_t>(index)];
}

Material *GetRandomMaterial()
{
    int randomIndex = rand() % static_cast<int>(MatIndex::Count);
    return GetMaterial(static_cast<MatIndex>(randomIndex));
}
