#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GL{
class Model : public IMeshRenderable{
public:
    Model() = default;
    Model(const char *filePath);
    ~Model() = default;

    void Draw() const override;

    std::vector<GL::Mesh*> GetMeshes() override { 
        std::vector<GL::Mesh*> ptrMeshes;
        for(auto& mesh : meshes) ptrMeshes.push_back(&mesh);
        return ptrMeshes;
    }
    
protected:
    std::vector<Mesh> meshes;
    std::string directory;

private:
    void ProcessNode(const aiNode* node, const aiScene* scene);
    GL::Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
    std::vector<GL::TextureObj> LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);
};
}