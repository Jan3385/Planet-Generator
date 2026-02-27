#include "Renderer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_access.hpp>

#include "Debug/Logger.h"
#include "Engine/Engine.h"
#include "Component/BaseComponent.h"
#include "Component/Planet/PlanetGenComponent.h"
#include "Math/Random.h"

void UpdateViewport(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    GameEngine::renderer->StoreWindowSize(width, height);
}

void Renderer::StoreWindowSize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
}

void Renderer::ObjectGeometryRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos, Frustum &frustumPlanes)
{
    for(auto& callback : renderCallbacks) {
        if(callback->IsInsideFrustum(frustumPlanes))
            callback->Render(projection, view);
    }
}

void Renderer::ObjectsSpecialRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos, Frustum &frustumPlanes)
{
    Component::SkyboxRender* skybox = GameEngine::currentLevel->GetSkybox();
    if(skybox) {
        if(this->isWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // temp disable wireframe mode for skybox
        skybox->Render(projection, view);
        if(this->isWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    for(auto& callback : noLightRenderCallbacks) {
        if(callback->IsInsideFrustum(frustumPlanes))
            callback->Render(projection, view);
    }

    // sort transparent objects
    std::sort(transparentRenderCallbacks.begin(), transparentRenderCallbacks.end(),
        [camPos](IRendererCallback* a, IRendererCallback* b) {
            Component::BaseMeshRender* renderA = dynamic_cast<Component::BaseMeshRender*>(a);
            Component::BaseMeshRender* renderB = dynamic_cast<Component::BaseMeshRender*>(b);

            glm::vec3 posA = renderA->GetOwner()->GetComponent<Component::Transform>()->GetPos();
            glm::vec3 posB = renderB->GetOwner()->GetComponent<Component::Transform>()->GetPos();

            float distA = glm::dot(camPos - posA, camPos - posA);
            float distB = glm::dot(camPos - posB, camPos - posB);

            return distA > distB; // farthest first
        }
    );

    for(auto& callback : transparentRenderCallbacks) {
        if(callback->IsInsideFrustum(frustumPlanes))
            callback->Render(projection, view);
    }
}

void Renderer::ObjectsVelocityRenderPass(Frustum &frustumPlanes)
{
    if(!this->taa) return;

    for(auto& callback : renderCallbacks) {
        if(callback->IsInsideFrustum(frustumPlanes))
            callback->RenderVelocity(this->taa->velocityShader);
    }

    for(auto& callback : noLightRenderCallbacks) {
        if(callback->IsInsideFrustum(frustumPlanes))
            callback->RenderVelocity(this->taa->velocityShader);
    }
}

void Renderer::ImGuiRenderPass()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if(Input::GetCursorMode() == Input::CursorMode::Normal) {
        this->DrawImGuiWindows();
    }

    for (auto callback : imguiCallbacks)
    {
        callback->ImGuiUpdate();
    }
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::GLDrawScreenQuad()
{
    if(this->isWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // temp disable wireframe mode
    
    this->quadVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if(this->isWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::DrawImGuiWindows()
{
    ImGui::Begin("Basic Window");
    ImGui::Text("Delta Time: %.3f ms", GameEngine::instance->DeltaTime() * 1000.0f);
    ImGui::Text("Immediate FPS: %.1f", GameEngine::instance->GetFPS());
    ImGui::Text("Avg FPS: %.1f", GameEngine::instance->GetAvgFPS());
    if(ImGui::Button("Toggle Wireframe")){
        this->WireframeMode(!this->isWireframeMode);
    }
    static bool isBackfaceCullingEnabled = true;
    if(ImGui::Button("Toggle Backface Culling")){
        this->BackfaceCulling(!isBackfaceCullingEnabled);
        isBackfaceCullingEnabled = !isBackfaceCullingEnabled;
    }
    static float gammaValue = 2.2f;
    if(ImGui::DragFloat("Gamma", &gammaValue, 0.1f, 0.1f, 10.0f)){
        this->SetGammaCorrection(gammaValue);
    }
    static float exposureValue = 1.0f;
    if(ImGui::DragFloat("Exposure", &exposureValue, 0.1f, 0.1f, 10.0f)){
        GL::Shader::UpdateShaderVariable("float exposure", exposureValue);
    }
    static float aaThreshold = 0.15f;
    if(ImGui::DragFloat("AntiAliasing threshold", &aaThreshold, 0.001f, 0.001f, 1.0f)){
        GL::Shader::UpdateShaderVariable("float aaThreshold", aaThreshold);
    }
    static float sunDir[3] = {-0.8f, -0.8f, 0.3f};
    if(ImGui::DragFloat3("Light Direction", &sunDir[0], 0.1f, -1.0f, 1.0f)){
        GameEngine::lighting->SetDirectionalLightSourceDirection(glm::vec3(sunDir[0], sunDir[1], sunDir[2]));
    }
    glm::vec3 ambientColor = GameEngine::lighting->GetAmbientColor();
    float ambientColorArr[3] = {ambientColor.r, ambientColor.g, ambientColor.b};
    if(ImGui::ColorEdit3("Ambient Color", ambientColorArr)){
        GameEngine::lighting->SetAmbientColor(glm::vec3(ambientColorArr[0], ambientColorArr[1], ambientColorArr[2]));
    }
    float ambientIntensity = GameEngine::lighting->GetAmbientIntensity();
    if(ImGui::DragFloat("Ambient Intensity", &ambientIntensity, 0.01f, 0.0f, 10.0f)){
        GameEngine::lighting->SetAmbientIntensity(ambientIntensity);
    }
    
    const char *renderModes[] = { "Standard", "Normal", "Albedo", "Specular" };
    static int currentRenderMode = 0;
    if(ImGui::Combo("Render Mode", &currentRenderMode, renderModes, IM_ARRAYSIZE(renderModes))){
        this->SetSpecialRenderMode(static_cast<SpecialRenderMode>(currentRenderMode));
    }

    ImGui::End();
}

Renderer::Renderer(uint16_t width, uint16_t height, EngineConfig::AntiAliasingMethod antialiasing, float gamma)
 : antiAliasingMethod(antialiasing)
{
    this->SetupShaderValues();

    this->window = glfwCreateWindow(width, height, "Planet renderer", nullptr, nullptr);

    if(!this->window) {
        Debug::LogFatal("Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, UpdateViewport);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Debug::LogFatal("Failed to initialize GLAD");
        return;
    }

    glEnable(GL_DEPTH_TEST);
    
    this->BackfaceCulling(true);
    glCullFace(GL_BACK);
    this->SetReverseFaceCulling(false);

    this->SetGammaCorrection(gamma);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //GL_LINE
    // glPointSize(7.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    this->defaultLightShader = std::move(GL::BasicShaderProgram("LightedShader"));
    
    this->defaultColorShader = std::move(GL::BasicShaderProgram("ColorShader"));
    this->skyboxShader = std::move(GL::BasicShaderProgram("SkyboxShader"));

    this->quadVBO = new GL::Buffer<float, GL_ARRAY_BUFFER>("Quad VBO");
    this->quadVBO->SetData(
        std::vector<float>{
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    }, GL_STATIC_DRAW);
    this->quadVAO = new GL::VertexArray("Quad VAO");
    this->quadVAO->Bind();
    this->quadVBO->Bind();
    this->quadVAO->AddAttribute<float, float>(0, 2, *this->quadVBO, GL_FALSE, 0, 0, 4 * sizeof(float));
    this->quadVAO->AddAttribute<float, float>(1, 2, *this->quadVBO, GL_FALSE, 2 * sizeof(float), 0, 4 * sizeof(float));
    this->quadVAO->Unbind();

    this->lightPassShader = new GL::BasicShaderProgram("LightPassShader");
    GameEngine::lighting->RegisterShaderLightUpdateCallback(this->lightPassShader);
    this->lightPassShader->Use();
    this->lightPassShader->SetInt("gPosition", 0);
    this->lightPassShader->SetInt("gNormal", 1);
    this->lightPassShader->SetInt("gAlbedoSpec", 2);

    this->postProcessShader = new GL::BasicShaderProgram("post-processing/PostProcessShader");
    this->postProcessShader->Use();
    this->postProcessShader->SetInt("screenTexture", 0);

    this->geometryFramebuffer = new GL::FrameBuffer(GL::DepthBufferMode::RenderBuffer);
    // position color buffer
    this->geometryFramebuffer->AddBufferTexture(GL_RGBA16F, GL::TextureFormat::RGBA, GL_FLOAT);
    // normal color buffer
    this->geometryFramebuffer->AddBufferTexture(GL_RGBA16F, GL::TextureFormat::RGBA, GL_FLOAT);
    // color + specular - albedo buffer
    this->geometryFramebuffer->AddBufferTexture(GL_SRGB8_ALPHA8, GL::TextureFormat::RGBA, GL_UNSIGNED_BYTE);
    this->geometryFramebuffer->CompleteSetup();

    this->postProcessFramebuffer = new GL::FrameBuffer(GL::DepthBufferMode::RenderBuffer);
    this->postProcessFramebuffer->AddBufferTexture(GL_RGB16F, GL::TextureFormat::RGB, GL_FLOAT);
    this->postProcessFramebuffer->CompleteSetup();

    if(antialiasing == EngineConfig::AntiAliasingMethod::MLAA) {
        Debug::LogTrace("Initializing SMAA anti-aliasing components");

        this->mlaa = new MLAA_Components();

        this->mlaa->edgeFBO = GL::FrameBuffer(GL::DepthBufferMode::None);
        this->mlaa->edgeFBO.AddBufferTexture(GL_RG16F, GL::TextureFormat::RED_GREEN, GL_FLOAT);
        this->mlaa->edgeFBO.CompleteSetup();
        this->mlaa->edgeShader = GL::BasicShaderProgram("post-processing/PostProcessShader.vert", "post-processing/EdgeDetectionShader.frag", "Edge Detection Pass FBO");
        this->mlaa->edgeShader.SetInt("screenTexture", 0);

        this->mlaa->blendWeightFBO = GL::FrameBuffer(GL::DepthBufferMode::None);
        this->mlaa->blendWeightFBO.AddBufferTexture(GL_RG16F, GL::TextureFormat::RED_GREEN, GL_FLOAT);
        this->mlaa->blendWeightFBO.CompleteSetup();
        this->mlaa->blendWeightShader = GL::BasicShaderProgram("post-processing/PostProcessShader.vert", "post-processing/BlendWeightShader.frag", "Blend Weight Calculation Pass FBO");
        this->mlaa->blendWeightShader.SetInt("uEdgeTex", 0);

        this->mlaa->neighborhoodBlendingFBO = GL::FrameBuffer(GL::DepthBufferMode::None);
        this->mlaa->neighborhoodBlendingFBO.AddBufferTexture(GL_RGB16F, GL::TextureFormat::RGB, GL_FLOAT);
        this->mlaa->neighborhoodBlendingFBO.CompleteSetup();
        this->mlaa->neighborhoodBlendingShader = GL::BasicShaderProgram("post-processing/PostProcessShader.vert", "post-processing/NeighborhoodBlendingShader.frag", "Neighborhood Blending Pass FBO");
        this->mlaa->neighborhoodBlendingShader.SetInt("screenTexture", 0);
        this->mlaa->neighborhoodBlendingShader.SetInt("uBlendWeightTex", 1);
    }
    if(antialiasing == EngineConfig::AntiAliasingMethod::TAA) {
        Debug::LogTrace("Initializing TAA anti-aliasing components");

        this->taa = new TAA_Components();

        this->taa->TAA_FBO1 = GL::FrameBuffer(GL::DepthBufferMode::None);
        this->taa->TAA_FBO1.AddBufferTexture(GL_RGB16F, GL::TextureFormat::RGB, GL_FLOAT);
        this->taa->TAA_FBO1.CompleteSetup();
        this->taa->TAA_FBO2 = GL::FrameBuffer(GL::DepthBufferMode::None);
        this->taa->TAA_FBO2.AddBufferTexture(GL_RGB16F, GL::TextureFormat::RGB, GL_FLOAT);
        this->taa->TAA_FBO2.CompleteSetup();
        this->taa->TAAShader = GL::BasicShaderProgram("post-processing/PostProcessShader.vert", "post-processing/TAABlendShader.frag", "TAA Blend Pass FBO");
        this->taa->TAAShader.SetInt("uCurrent", 0);
        this->taa->TAAShader.SetInt("uHistory", 1);
        this->taa->TAAShader.SetInt("uVelocity", 2);

        this->taa->velocityFBO = GL::FrameBuffer(GL::DepthBufferMode::None);
        this->taa->velocityFBO.AddBufferTexture(GL_RG16F, GL::TextureFormat::RED_GREEN, GL_FLOAT);
        this->taa->velocityFBO.CompleteSetup();
        this->taa->velocityShader = GL::BasicShaderProgram("post-processing/VelocityShader");
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        Debug::LogError("OpenGL error during renderer initialization: " + std::to_string(err));
    }
}

Renderer::~Renderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if(this->window != nullptr) {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }

    delete this->quadVBO;
    delete this->quadVAO;
    delete this->postProcessShader;
    delete this->lightPassShader;
    delete this->geometryFramebuffer;
    delete this->postProcessFramebuffer;

    if(this->mlaa) delete this->mlaa;
    if(this->taa) delete this->taa;
}

void Renderer::SetVSYNC(bool enabled)
{
    if(!enabled) glfwSwapInterval(0);
    else         glfwSwapInterval(1);
}

void Renderer::SetReverseFaceCulling(bool reversed)
{
    if(reversed) glFrontFace(GL_CW);
    else         glFrontFace(GL_CCW);
}

void Renderer::SetGammaCorrection(float value)
{
    GL::Shader::UpdateShaderVariable("float gamma", value);
}

void Renderer::Update()
{
    static bool firstFrame = true;

    // Camera setup
    Component::Camera* camera = GameEngine::currentLevel->GetCamera();
    camera->SetAspectRatio(static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight));

    // bind N nearest point lights to the shader
    auto closestPLights = GameEngine::lighting->GetClosestPointLights(camera->GetPosition());
    int pointLightCount = 0;
    this->GetLightPassShader().Use();
    for (auto* pointLight : closestPLights) {
        if (pointLight != nullptr) {
            pointLight->Bind(this->GetLightPassShader(), pointLightCount);
            pointLightCount++;
        }
    }
    this->GetLightPassShader().SetInt("numPointLights", pointLightCount);

    // setup projections etc
    glm::mat4 projection = camera->GetProjection();

    // For TAA jitter projection
    if(this->taa){
        this->taa->frameIndex++;
        projection = this->JitterProjection(projection, this->taa->frameIndex);
    }

    glm::mat4 view = camera->GetView();
    GL::Shader::UpdateShaderVariable("mat4 projection", projection);
    GL::Shader::UpdateShaderVariable("mat4 view", view);

    if(firstFrame && this->taa){
        this->taa->previousProjection = projection;
        this->taa->previousView = view;
        GL::Shader::UpdateShaderVariable("mat4 previousProjection", this->taa->previousProjection);
        GL::Shader::UpdateShaderVariable("mat4 previousView", this->taa->previousView);
    }
    
    glm::vec3 camPos = camera->GetPosition();
    GL::Shader::UpdateShaderVariable("vec3 viewPos", camPos);

    glm::vec2 screenSize = glm::vec2(this->windowWidth, this->windowHeight);
    glm::vec2 inverseScreenSize = 1.0f / screenSize;
    GL::Shader::UpdateShaderVariable("vec2 inverseScreenSize", inverseScreenSize);
    GL::Shader::UpdateShaderVariable("vec2 screenSize", screenSize);

    Frustum frustumPlanes = CalculateFrustumPlanes(projection, view);

    uint32_t renderClearFlags = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    if(this->isWireframeMode) renderClearFlags |= GL_COLOR_BUFFER_BIT;

    // 1. Geometry pass
    glDisable(GL_BLEND);
    this->geometryFramebuffer->UpdateSize(screenSize);
    this->geometryFramebuffer->BindShaderFBO();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(renderClearFlags);
    ObjectGeometryRenderPass(projection, view, camPos, frustumPlanes);
    this->geometryFramebuffer->UnbindShaderFBO();

    // 2. Light pass
    glClear(GL_DEPTH_BUFFER_BIT);
    this->lightPassShader->Use();
    this->geometryFramebuffer->BindTextures();
    this->postProcessFramebuffer->UpdateSize(screenSize);
    this->postProcessFramebuffer->BindShaderFBO();
    glClear(renderClearFlags);
    this->GLDrawScreenQuad();

    // 2.5 No-light objects pass
    glEnable(GL_BLEND);
    this->geometryFramebuffer->CopyDepthToFBO(*this->postProcessFramebuffer);
    this->postProcessFramebuffer->BindShaderFBO();
    ObjectsSpecialRenderPass(projection, view, camPos, frustumPlanes);
    GL::FrameBuffer *lastFBO = this->postProcessFramebuffer;
    
    // 3. Post-processing pass
    glDisable(GL_DEPTH_TEST);

    // 3.1 MLAA if enabled
    if(this->mlaa){
        // 1. Edge detection pass
        this->mlaa->edgeFBO.UpdateSize(screenSize);
        this->mlaa->edgeFBO.BindShaderFBO();
        glClear(GL_COLOR_BUFFER_BIT);
        this->mlaa->edgeShader.Use();
        this->postProcessFramebuffer->BindTextures();
        this->GLDrawScreenQuad();
        this->mlaa->edgeFBO.UnbindShaderFBO();

        // 2. Blend weight calculation pass
        this->mlaa->blendWeightFBO.UpdateSize(screenSize);
        this->mlaa->blendWeightFBO.BindShaderFBO();
        glClear(GL_COLOR_BUFFER_BIT);
        this->mlaa->blendWeightShader.Use();
        this->mlaa->edgeFBO.BindTextures();
        this->GLDrawScreenQuad();
        this->mlaa->blendWeightFBO.UnbindShaderFBO();

        // 3. Neighborhood blending pass
        this->mlaa->neighborhoodBlendingFBO.UpdateSize(screenSize);
        this->mlaa->neighborhoodBlendingFBO.BindShaderFBO();
        glClear(GL_COLOR_BUFFER_BIT);
        this->mlaa->neighborhoodBlendingShader.Use();
        this->postProcessFramebuffer->BindTextures();
        this->mlaa->blendWeightFBO.BindTextures(1);
        this->GLDrawScreenQuad();
        this->mlaa->neighborhoodBlendingFBO.UnbindShaderFBO();

        lastFBO = &this->mlaa->neighborhoodBlendingFBO;
    }
    // 3.1 TAA if enabled
    else if(this->taa){
        // render velocity to a texture
        glEnable(GL_DEPTH_TEST);
        this->taa->velocityFBO.UpdateSize(screenSize);
        this->taa->velocityFBO.BindShaderFBO();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        this->taa->velocityShader.Use();
        ObjectsVelocityRenderPass(frustumPlanes);
        this->taa->velocityFBO.UnbindShaderFBO();
        glDisable(GL_DEPTH_TEST);

        static bool flip = false;
        GL::FrameBuffer *writeFBO = flip ? &this->taa->TAA_FBO1    : &this->taa->TAA_FBO2;
        GL::FrameBuffer *readFBO =  flip ? &this->taa->TAA_FBO2 : &this->taa->TAA_FBO1;
        flip = !flip;

        writeFBO->UpdateSize(screenSize);
        readFBO->UpdateSize(screenSize);

        writeFBO->BindShaderFBO();
        glClear(GL_COLOR_BUFFER_BIT);
        this->taa->TAAShader.Use();
        this->postProcessFramebuffer->BindTextures(0);
        readFBO->BindTextures(1);
        this->taa->velocityFBO.BindTextures(2); // velocity buffer
        this->GLDrawScreenQuad();
        writeFBO->UnbindShaderFBO();

        this->taa->previousProjection = projection;
        this->taa->previousView = view;
        GL::Shader::UpdateShaderVariable("mat4 previousProjection", this->taa->previousProjection);
        GL::Shader::UpdateShaderVariable("mat4 previousView", this->taa->previousView);

        lastFBO = writeFBO;
    }

    // 3.2 Final post-processing
    lastFBO->UnbindShaderFBO();
    this->postProcessShader->Use();
    lastFBO->BindTextures(0);
    this->GLDrawScreenQuad();

    glEnable(GL_DEPTH_TEST);

    // 4. Render ImGui
    ImGuiRenderPass();

    // 5. Swap buffers
    glfwSwapBuffers(window);

    firstFrame = false;
}

void Renderer::WireframeMode(bool enabled)
{
    if(enabled) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    this->isWireframeMode = enabled;
}

void Renderer::BackfaceCulling(bool enabled)
{
    if(enabled) glEnable(GL_CULL_FACE);
    else        glDisable(GL_CULL_FACE);
}

void Renderer::SetSpecialRenderMode(SpecialRenderMode mode) const
{
    GL::Shader::UpdateShaderVariable("int SpecialRenderMode", static_cast<int>(mode));
}

void Renderer::SetupShaderValues()
{
    GL::Shader::AddShaderConstant("SHADER_VERSION", "460");
    GL::Shader::AddShaderConstant("MAX_POINT_LIGHTS", std::to_string(Lighting::MAX_EFFECTING_POINT_LIGHTS));
    GL::Shader::AddShaderConstant("PI", "3.14159265359");
    GL::Shader::AddShaderConstant("LOW_POLY_FEEL", Lighting::LOW_POLY_LIGHTING_FEEL ? "1" : "0");
    GL::Shader::AddShaderConstant("PLANET_SCALE", std::to_string(Component::PlanetGen::PLANET_SCALE));
    GL::Shader::AddShaderConstant("FXAA_ANTIALIASING", 
        this->antiAliasingMethod == EngineConfig::AntiAliasingMethod::FXAA ? "1" : "0");

    GL::Shader::AddShaderVariable("mat4 projection", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("mat4 view", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("vec3 viewPos", glm::vec3(0.0f));

    GL::Shader::AddShaderVariable("float gamma", 2.2f);
    GL::Shader::AddShaderVariable("float exposure", 1.0f);
    GL::Shader::AddShaderVariable("vec2 inverseScreenSize", glm::vec2(0.0f));
    GL::Shader::AddShaderVariable("vec2 screenSize", glm::vec2(0.0f));
    GL::Shader::AddShaderVariable("float aaThreshold", 0.15f);
    GL::Shader::AddShaderVariable("mat4 previousProjection", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("mat4 previousView", glm::mat4(1.0f));

    GL::Shader::AddShaderVariable("int SpecialRenderMode", 0);

    Debug::LogSpam("Shader constants and variables initialized");
}

std::array<glm::vec4, 6> Renderer::CalculateFrustumPlanes(const glm::mat4 &projection, const glm::mat4 &view)
{
    glm::mat4 m = projection * view;
    std::array<glm::vec4, 6> planes;

    glm::vec4 row0 = glm::row(m, 0);
    glm::vec4 row1 = glm::row(m, 1);
    glm::vec4 row2 = glm::row(m, 2);
    glm::vec4 row3 = glm::row(m, 3);

    // Left
    planes[0] = row3 + row0;
    // Right
    planes[1] = row3 - row0;
    // Bottom
    planes[2] = row3 + row1;
    // Top
    planes[3] = row3 - row1;
    // Near
    planes[4] = row3 + row2;
    // Far
    planes[5] = row3 - row2;


    for (auto& plane : planes) {
        float length = glm::length(glm::vec3(plane));
        plane /= length;
    }
    
    return planes;
}

glm::mat4 Renderer::JitterProjection(const glm::mat4 &projection, const int frameIndex)
{
    glm::vec2 frameJitter = glm::vec2(
        (Math::Random::HaltonSequence(frameIndex % 16, 2) - 0.5f),
        (Math::Random::HaltonSequence(frameIndex % 16, 3) - 0.5f)
    );

    frameJitter = frameJitter / glm::vec2(this->windowWidth, this->windowHeight);

    glm::mat4 jitteredProjection = projection;
    constexpr float jiggleScale = 1.0f;
    jitteredProjection[2][0] += frameJitter.x * jiggleScale;
    jitteredProjection[2][1] += frameJitter.y * jiggleScale;
    return jitteredProjection;
}
