#include "Renderer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Debug/Logger.h"
#include "Engine/Engine.h"
#include "Component/BaseComponent.h"

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
    if(ImGui::Button("Toggle Backface Culling")){
        this->BackfaceCulling(!this->isBackfaceCullingEnabled);
    }
    ImGui::End();
}

Renderer::Renderer(uint16_t width, uint16_t height, bool multiSample)
{
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
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    if(multiSample) glEnable(GL_MULTISAMPLE);

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
    
    this->defaultColorShader = std::move(GL::BasicShaderProgram("BasicShader.vert", "ColorShader.frag", "Color Shader"));
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
}

void Renderer::SetVSYNC(bool enabled)
{
    if(!enabled) glfwSwapInterval(0);
    else glfwSwapInterval(1);
}

void Renderer::Update()
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Component::Camera* camera = GameEngine::currentLevel->GetCamera();
    camera->SetAspectRatio(static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight));

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();

    // sort transparent objects
    glm::vec3 camPos = camera->GetOwner()->GetComponent<Component::Transform>()->GetPos();
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

    this->isBackfaceCullingEnabled = enabled;
}
