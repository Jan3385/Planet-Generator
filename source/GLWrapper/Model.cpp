#include "Model.h"

#include "Debug/Logger.h"

GL::Model::Model(const char *filePath)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        filePath, 
        aiProcess_Triangulate | aiProcess_FlipUVs
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) [[unlikely]] {
        Debug::LogError("Asset import error: " + std::string(importer.GetErrorString()));
        return;
    }

    this->directory = std::string(filePath).substr(0, std::string(filePath).find_last_of('/'));

    this->ProcessNode(scene->mRootNode, scene);
}

void GL::Model::Draw() const
{
    for(const auto& mesh : this->meshes)
        mesh.Draw();
}

void GL::Model::ProcessNode(const aiNode *node, const aiScene *scene)
{
    // process meshes
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(std::move(this->ProcessMesh(mesh, scene)));
    }

    // process children
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

GL::Mesh GL::Model::ProcessMesh(const aiMesh *mesh, const aiScene *scene)
{
    std::vector<GL::VertexObj> vertices;
    std::vector<unsigned int> indices;
    std::vector<GL::TextureObj> textures;

    // vertex
    for (unsigned int i = 0; i < mesh->mNumVertices; i++){
        GL::VertexObj vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        
        if (mesh->mTextureCoords[0])
            vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.uv = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // material
    if(mesh->mMaterialIndex < scene->mNumMaterials){
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<GL::TextureObj> diffuseMaps = 
            this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<GL::TextureObj> specularMaps = 
            this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    return GL::Mesh(vertices, indices, textures);
}

std::vector<GL::TextureObj> GL::Model::LoadMaterialTextures(const aiMaterial *mat, aiTextureType type, const std::string &typeName)
{
    std::vector<GL::TextureObj> textures;
    
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);
        GL::TextureObj t;
        t.texture = std::move(GL::Texture(TextureWrapMode::Repeat, MipmapMode::Linear, true));
        t.texture.GenTexture(TextureFormat::RGB, GL_RGB, this->directory + '/' + str.C_Str());
        t.type = typeName;
        textures.push_back(std::move(t));
    }

    return textures;
}
