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
    static bool isGammaCorrectionEnabled = false;
    if(ImGui::Button("Toggle Gamma Correction")){
        this->SetGammaCorrection(!isGammaCorrectionEnabled);
        isGammaCorrectionEnabled = !isGammaCorrectionEnabled;
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

Renderer::Renderer(uint16_t width, uint16_t height, uint8_t MSAA_Samples)
{
    GL::Shader::AddShaderConstant("SHADER_VERSION", "460");
    GL::Shader::AddShaderConstant("MAX_POINT_LIGHTS", std::to_string(Lighting::MAX_EFFECTING_POINT_LIGHTS));
    GL::Shader::AddShaderConstant("LOW_POLY_FEEL", Lighting::LOW_POLY_LIGHTING_FEEL ? "1" : "0");
    GL::Shader::AddShaderConstant("PLANET_SCALE", std::to_string(Component::PlanetGen::PLANET_SCALE));

    GL::Shader::AddShaderVariable("mat4 projection", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("mat4 view", glm::mat4(1.0f));
    GL::Shader::AddShaderVariable("vec3 viewPos", glm::vec3(0.0f));

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

    this->SetGammaCorrection(false);

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
    GameEngine::lighting->RegisterShaderLightUpdateCallback(&this->defaultLightShader);
    
    this->defaultColorShader = std::move(GL::BasicShaderProgram("ColorShader"));

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

    this->quadShader = new GL::BasicShaderProgram("TextureQuadShader");
    this->quadShader->Use();
    this->quadShader->SetInt("screenTexture", 0);

    this->framebuffer = new GL::FrameBuffer<GL::FrameBufferColorType::Texture, GL::FrameBufferDepthStencilType::None>(MSAA_Samples);
    this->framebuffer->clearColor = glm::vec4(0.1f, 0.1f, 0.2f, 1.0f);
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
    delete this->quadShader;
    delete this->framebuffer;
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

void Renderer::SetGammaCorrection(bool enabled)
{
    if(enabled) glEnable(GL_FRAMEBUFFER_SRGB);
    else        glDisable(GL_FRAMEBUFFER_SRGB);
}

void Renderer::Update()
{
    framebuffer->UpdateSize(glm::uvec2(this->windowWidth, this->windowHeight));
    framebuffer->BindAndClear();

    Component::Camera* camera = GameEngine::currentLevel->GetCamera();
    camera->SetAspectRatio(static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight));

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();
    GL::Shader::UpdateShaderVariable("mat4 projection", projection);
    GL::Shader::UpdateShaderVariable("mat4 view", view);
    
    glm::vec3 camPos = camera->GetPosition();
    GL::Shader::UpdateShaderVariable("vec3 viewPos", camPos);

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

    for(auto& callback : renderCallbacks) {
        callback->Render(projection, view);
    }

    for(auto& callback : transparentRenderCallbacks) {
        callback->Render(projection, view);
    }

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
    
    bool wireframeMode = this->isWireframeMode;
    if(wireframeMode) this->WireframeMode(false);
    framebuffer->Render(this->quadShader, this->quadVAO);
    if(wireframeMode) this->WireframeMode(true);

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
