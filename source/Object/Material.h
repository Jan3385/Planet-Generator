#pragma once

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

enum class MatIndex {
    Obsidian,
    Ruby,
    Brass,
    Bronze,
    Chrome,
    Copper,
    Gold,
    Silver,
    BlackPlastic,
    WhitePlastic,
    RedPlastic,
    RedRubber,
    Count
};

Material* GetMaterial(MatIndex index);

extern Material CommonMaterials[];
