#include "SkyboxRenderComponent.h"

#include "Engine/Engine.h"
#include "Generator/MeshGenerator.h"

Component::SkyboxRender::SkyboxRender(Object::BaseObject *owner)
    : BaseMeshRender(owner)
{
    this->SetRenderShader(&GameEngine::renderer->GetSkyboxShader());
    this->cubeMesh = MeshGenerator::GenerateCubeMesh();
}

/**
 * @brief Loads cubemap textures from file paths
 * @param filePaths Array of 6 file paths for the cubemap faces
 * @param blurred if the textures should be blurred
 * @note Order of images: right, left, bottom, top, front, back
 */
void Component::SkyboxRender::LoadCubemap(const std::string filePaths[6], bool blurred)
{
    this->cubemap = GL::Cubemap(filePaths, blurred);
}

void Component::SkyboxRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->cubemap.isInitialized()) return;

    Renderer::SetReverseFaceCulling(true);

    glDepthMask(GL_FALSE);
    this->renderShader->Use();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    this->renderShader->SetMat4("view", viewNoTranslation);

    this->cubeMesh->Bind();

    this->cubemap.Bind();

    this->cubeMesh->Draw();
    Renderer::SetReverseFaceCulling(false);
    glDepthMask(GL_TRUE);
}

void Component::SkyboxRender::Awake()
{
    //if(!this->transform)
    //    this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    GameEngine::renderer->AddRenderCallback(this);
}

void Component::SkyboxRender::OnDestroy()
{
    GameEngine::renderer->RemoveRenderCallback(this);
}
