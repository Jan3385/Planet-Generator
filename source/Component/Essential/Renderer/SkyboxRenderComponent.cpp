#include "SkyboxRenderComponent.h"

#include "Engine/Engine.h"
#include "Generator/MeshGenerator.h"

Component::SkyboxRender::SkyboxRender(Object::BaseObject *owner)
    : BaseMeshRender(owner)
{
    this->SetRenderShader(&GameEngine::renderer->GetSkyboxShader());
    this->cubeMesh = MeshGenerator::GenerateCubeMesh();

    if(!GameEngine::currentLevel->GetSkybox())
        GameEngine::currentLevel->SetSkybox(this);
    else{
        Debug::LogWarn("Multiple SkyboxRender components exist! Only one skybox should be present");
    }
}

/**
 * @brief Loads cubemap textures from file paths
 * @param filePaths Array of 6 file paths for the cubemap faces
 * @param blurred if the textures should be blurred
 * @param sRGB if the textures should be loaded within sRGB color space
 * @note Order of images: right, left, bottom, top, front, back
 */
void Component::SkyboxRender::LoadCubemap(const std::string filePaths[6], bool blurred, bool sRGB)
{
    this->cubemap = GL::Cubemap(filePaths, blurred, sRGB);
}

void Component::SkyboxRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->cubemap.isInitialized()) return;

    Renderer::SetReverseFaceCulling(true);
    glDepthFunc(GL_LEQUAL);

    this->renderShader->Use();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    this->renderShader->SetMat4("view", viewNoTranslation);

    this->cubeMesh->Bind();

    this->cubemap.Bind();

    this->cubeMesh->Draw();
    
    Renderer::SetReverseFaceCulling(false);
    glDepthFunc(GL_LESS);
}

void Component::SkyboxRender::RenderVelocity(GL::Shader &s)
{
    if(!this->transform || !this->cubeMesh) return;

    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);
    s.SetMat4("prevTransform", this->prevMatrixTransform);
    this->prevMatrixTransform = model;

    this->cubeMesh->Bind();
    this->cubeMesh->Draw();
}