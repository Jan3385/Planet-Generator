#include "Renderer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Debug/Logger.h"
#include "Engine/Engine.h"
#include "Component/BaseComponent.h"
#include "Component/Planet/PlanetGenComponent.h"

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

void Renderer::ObjectGeometryRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos)
{
    for(auto& callback : renderCallbacks) {
        callback->Render(projection, view);
    }
}

void Renderer::ObjectsSpecialRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos)
{
    Component::SkyboxRender* skybox = GameEngine::currentLevel->GetSkybox();
    if(skybox) skybox->Render(projection, view);

    for(auto& callback : noLightRenderCallbacks) {
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
        callback->Render(projection, view);
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

void Renderer::DrawImGuiWindows()
{
    ImGui::Begin("Basic Window");
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
    static float sunDir[3] = {-0.8f, 0.3f, 0.3f};
    if(ImGui::DragFloat3("Light Direction", &sunDir[0], 0.1f, -1.0f, 1.0f)){
        GameEngine::lighting->SetDirectionalLightSourceDirection(glm::vec3(sunDir[0], sunDir[1], sunDir[2]));
    }
    glm::vec3 ambientColor = GameEngine::lighting->GetAmbientColor();
    float ambientColorArr[3] = {ambientColor.r, ambientColor.g, ambientColor.b};
    if(ImGui::ColorEdit3("Ambient Color", ambientColorArr)){
        GameEngine::lighting->SetAmbientColor(glm::vec3(ambientColorArr[0], ambientColorArr[1], ambientColorArr[2]));
    }
    ImGui::End();
}

Renderer::Renderer(uint16_t width, uint16_t height, uint8_t MSAA_Samples, float gamma)
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

    if(MSAA_Samples > 1) glEnable(GL_MULTISAMPLE);

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

    this->postProcessShader = new GL::BasicShaderProgram("PostProcessShader");
    this->postProcessShader->Use();
    this->postProcessShader->SetInt("screenTexture", 0);

    this->geometryFramebuffer = new GL::FrameBuffer();
    // position color buffer
    this->geometryFramebuffer->AddBufferTexture(GL_RGBA16F, GL::TextureFormat::RGBA, GL_FLOAT);
    // normal color buffer
    this->geometryFramebuffer->AddBufferTexture(GL_RGBA16F, GL::TextureFormat::RGBA, GL_FLOAT);
    // color + specular - albedo buffer
    this->geometryFramebuffer->AddBufferTexture(GL_RGBA, GL::TextureFormat::RGBA, GL_UNSIGNED_BYTE);
    this->geometryFramebuffer->CompleteSetup();

    this->postProcessFramebuffer = new GL::FrameBuffer();
    this->postProcessFramebuffer->AddBufferTexture(GL_SRGB8, GL::TextureFormat::RGBA, GL_UNSIGNED_BYTE);
    this->postProcessFramebuffer->CompleteSetup();
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
    Component::Camera* camera = GameEngine::currentLevel->GetCamera();
    camera->SetAspectRatio(static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight));

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();
    GL::Shader::UpdateShaderVariable("mat4 projection", projection);
    GL::Shader::UpdateShaderVariable("mat4 view", view);
    
    glm::vec3 camPos = camera->GetPosition();
    GL::Shader::UpdateShaderVariable("vec3 viewPos", camPos);

    // 1. Geometry pass
    glDisable(GL_BLEND);
    this->geometryFramebuffer->UpdateSize(glm::uvec2(this->windowWidth, this->windowHeight));
    this->geometryFramebuffer->BindShaderFBO();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    ObjectGeometryRenderPass(projection, view, camPos);
    this->geometryFramebuffer->UnbindShaderFBO();

    // 2. Light pass
    glClear(GL_DEPTH_BUFFER_BIT);
    this->lightPassShader->Use();
    this->geometryFramebuffer->BindTextures();
    this->postProcessFramebuffer->UpdateSize(glm::uvec2(this->windowWidth, this->windowHeight));
    this->postProcessFramebuffer->BindShaderFBO();
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    this->quadVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 2.5 No-light objects pass
    glEnable(GL_BLEND);
    this->geometryFramebuffer->CopyDepthToFBO(*this->postProcessFramebuffer);
    this->postProcessFramebuffer->BindShaderFBO();
    ObjectsSpecialRenderPass(projection, view, camPos);
    this->postProcessFramebuffer->UnbindShaderFBO();

    // 3. Post-processing pass
    this->postProcessShader->Use();
    this->postProcessFramebuffer->BindTextures();
    this->quadVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 4. Render ImGui
    ImGuiRenderPass();

    // 5. Swap buffers
    glfwSwapBuffers(window);
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

void Renderer::SetupShaderValues()
{
    GL::Shader::AddShaderConstant("SHADER_VERSION", "460");
    GL::Shader::AddShaderConstant("MAX_POINT_LIGHTS", std::to_string(Lighting::MAX_EFFECTING_POINT_LIGHTS));
    GL::Shader::AddShaderConstant("LOW_POLY_FEEL", Lighting::LOW_POLY_LIGHTING_FEEL ? "1" : "0");
    GL::Shader::AddShaderConstant("PLANET_SCALE", std::to_string(Component::PlanetGen::PLANET_SCALE));

    GL::Shader::AddShaderVariable("mat4 projection", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("mat4 view", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("vec3 viewPos", glm::vec3(0.0f));

    GL::Shader::AddShaderVariable("float gamma", 2.2f);
    GL::Shader::AddShaderVariable("float exposure", 1.0f);
}
