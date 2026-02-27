#pragma once

#include "GLWrapper/Shader.h"
#include <glm/glm.hpp>

struct Material {
    glm::vec3 color;
    float metallic;
    float roughness;

    void Bind(GL::Shader& shader) const{
        shader.SetVec4("material.color", glm::vec4(this->color, 1.0f));
        shader.SetVec4("material.MetalRought", glm::vec4(this->metallic, this->roughness, 0, 0));
    }
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
Material* GetRandomMaterial();

extern Material CommonMaterials[];
