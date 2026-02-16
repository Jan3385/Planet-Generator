#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/FrameBuffer.h"

#include "Engine/Config.h"

#include <vector>

class Input;

namespace Component {
    class IImGuiUpdatable;
};

class Renderer{
public:
    struct IRendererCallback{
    protected:
        virtual void Render(glm::mat4 &projection, glm::mat4 &view) = 0;
        virtual ~IRendererCallback() = default;

        friend class Renderer;
    };
    struct MLAA_Components{
        GL::FrameBuffer edgeFBO;
        GL::BasicShaderProgram edgeShader;
        GL::FrameBuffer blendWeightFBO;
        GL::BasicShaderProgram blendWeightShader;
        GL::FrameBuffer neighborhoodBlendingFBO;
        GL::BasicShaderProgram neighborhoodBlendingShader;
    };

    Renderer(uint16_t width, uint16_t height, EngineConfig::AntiAliasingMethod antialiasing, float gamma);
    ~Renderer();

    void AddRenderCallback(IRendererCallback* callback) {
        renderCallbacks.push_back(callback);
    }
    void RemoveRenderCallback(IRendererCallback* callback) {
        std::erase(renderCallbacks, callback);
    }
    void AddTransparentRenderCallback(IRendererCallback* callback) {
        transparentRenderCallbacks.push_back(callback);
    }
    void RemoveTransparentRenderCallback(IRendererCallback* callback) {
        std::erase(transparentRenderCallbacks, callback);
    }
    void AddNoLightRenderCallback(IRendererCallback* callback) {
        noLightRenderCallbacks.push_back(callback);
    }
    void RemoveNoLightRenderCallback(IRendererCallback* callback) {
        std::erase(noLightRenderCallbacks, callback);
    }

    bool ShouldClose() const { return glfwWindowShouldClose(this->window); }

    static void SetVSYNC(bool enabled);
    static void SetReverseFaceCulling(bool reversed);
    static void SetGammaCorrection(float value);

    void Update();
    void WireframeMode(bool enabled);
    void BackfaceCulling(bool enabled);

    void StoreWindowSize(int width, int height);

    GL::BasicShaderProgram& GetDefaultLightShader() { return this->defaultLightShader; }
    GL::BasicShaderProgram& GetDefaultColorShader() { return this->defaultColorShader; }
    GL::BasicShaderProgram& GetSkyboxShader() { return this->skyboxShader; }
    GL::BasicShaderProgram& GetLightPassShader() { return *this->lightPassShader; }

    void AddImGuiCallback(Component::IImGuiUpdatable* callback) {
        imguiCallbacks.push_back(callback);
    }
    void RemoveImGuiCallback(Component::IImGuiUpdatable* callback) {
        std::erase(imguiCallbacks, callback);
    }

    EngineConfig::AntiAliasingMethod GetAntiAliasingMethod() const { return this->antiAliasingMethod; }
protected:
    void ObjectGeometryRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos);
    void ObjectsSpecialRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos);
    void ImGuiRenderPass();

    void SetupShaderValues();

    GL::VertexArray *quadVAO;
    GL::Buffer<float, GL_ARRAY_BUFFER> *quadVBO;

    GL::BasicShaderProgram *lightPassShader;
    GL::BasicShaderProgram *postProcessShader;

    GL::FrameBuffer *geometryFramebuffer;
    GL::FrameBuffer *postProcessFramebuffer;
private:
    bool isWireframeMode = false;
    EngineConfig::AntiAliasingMethod antiAliasingMethod;

    void GLDrawScreenQuad();

    GL::BasicShaderProgram defaultLightShader;
    GL::BasicShaderProgram defaultColorShader;
    GL::BasicShaderProgram skyboxShader;

    void DrawImGuiWindows();
    GLFWwindow* window = nullptr;
    std::vector<IRendererCallback*> renderCallbacks;
    std::vector<IRendererCallback*> transparentRenderCallbacks;
    std::vector<IRendererCallback*> noLightRenderCallbacks;

    std::vector<Component::IImGuiUpdatable*> imguiCallbacks;

    MLAA_Components *mlaa = nullptr;

    int windowWidth = 800;
    int windowHeight = 600;

    friend Input;
};