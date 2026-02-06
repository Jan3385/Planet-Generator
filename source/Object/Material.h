#pragma once

#include "GLWrapper/Shader.h"
#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    void Bind(GL::Shader& shader) const{
        shader.SetVec4("material.ambient", glm::vec4(this->ambient, 1.0f));
        shader.SetVec4("material.diffuse", glm::vec4(this->diffuse, 1.0f));
        shader.SetVec4("material.specular", glm::vec4(this->specular, 1.0f));
        shader.SetVec4("material.shininess", glm::vec4(this->shininess, this->shininess, this->shininess, 1.0f));
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

extern Material CommonMaterials[];
