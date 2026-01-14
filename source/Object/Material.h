#pragma once

#include "GLWrapper/Shader.h"
#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    void Bind(GL::Shader& shader) const{
        shader.SetVec3("material.ambient", this->ambient);
        shader.SetVec3("material.diffuse", this->diffuse);
        shader.SetVec3("material.specular", this->specular);
        shader.SetFloat("material.shininess", this->shininess);
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
