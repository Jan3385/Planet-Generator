#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/FrameBuffer.h"
#include "GLWrapper/Mesh.h"

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
        /// @brief Standard render pass
        /// @param projection 
        /// @param view 
        virtual void Render(glm::mat4 &projection, glm::mat4 &view) = 0;

        /// @brief TAA velocity render pass
        /// @param s Shader used
        virtual void RenderVelocity(GL::Shader &s) = 0;

        /// @brief Depth-only render pass
        /// @param s Shader used
        virtual void RenderDepthOnly(GL::Shader &s) = 0;

        /// @brief Checks if the object is inside the view frustum for culling
        /// @param frustumPlanes Array of 6 planes representing the view frustum
        /// @return true if the object is inside the frustum, false otherwise
        virtual bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) = 0;

        /// @brief Gets the centroid and radius of the object for frustum culling
        /// @param centroid Output parameter for the centroid of the object
        /// @param radius   Output parameter for the radius of the object
        /// @param meshIndex Input parameter for the index of the mesh
        /// @return true if the object data could be loaded, false otherwise
        virtual bool GetFrustumData(glm::vec3 &centroid, double &radius, size_t meshIndex) = 0;

        /// @brief Gets the position of the object for sorting transparent objects
        virtual glm::vec3 GetPosition() const = 0;

        virtual ~IRendererCallback() = default;

        friend class Renderer;
    };
    struct SSAO_Components{
        GL::Texture ssaoNoiseTexture;

        GL::FrameBuffer ssaoFBO;
        GL::BasicShaderProgram ssaoShader;
        GL::FrameBuffer ssaoBlurFBO;
        GL::BasicShaderProgram ssaoBlurShader;
    };
    struct MLAA_Components{
        GL::FrameBuffer edgeFBO;
        GL::BasicShaderProgram edgeShader;
        GL::FrameBuffer blendWeightFBO;
        GL::BasicShaderProgram blendWeightShader;
        GL::FrameBuffer neighborhoodBlendingFBO;
        GL::BasicShaderProgram neighborhoodBlendingShader;
    };
    struct TAA_Components{
        unsigned int frameIndex = 0;
        GL::FrameBuffer TAA_FBO1;
        GL::BasicShaderProgram TAAShader;

        GL::FrameBuffer TAA_FBO2;

        GL::FrameBuffer velocityFBO;
        GL::BasicShaderProgram velocityShader;

        glm::mat4 previousProjection;
        glm::mat4 previousView;
    };
    enum class SpecialRenderMode : uint8_t {
        Standard = 0,
        Normal = 1,
        Albedo = 2,
        Specular = 3
    };

    using Frustum = std::array<glm::vec4, 6>;

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
    void RenderShadowMap(GL::Shader &s, Frustum *frustumPlanes);

    void WireframeMode(bool enabled);
    void BackfaceCulling(bool enabled);
    void ShowFrustumColliders(bool enabled) { this->showFrustumColliders = enabled; }
    void SetSpecialRenderMode(SpecialRenderMode mode) const;

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

    glm::vec2 GetScreenSize() const { return glm::vec2(this->windowWidth, this->windowHeight); }

    static std::array<glm::vec4, 6> CalculateFrustumPlanes(const glm::mat4& projection, const glm::mat4& view);
protected:
    void ObjectGeometryRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos, Frustum *frustumPlanes);
    void ObjectsSpecialRenderPass(glm::mat4 &projection, glm::mat4 &view, glm::vec3 &camPos, Frustum *frustumPlanes);
    void ObjectsVelocityRenderPass(Frustum *frustumPlanes);
    void ObjectsFrustumDebugRenderPass();
    void ImGuiRenderPass();

    std::shared_ptr<GL::Mesh> debugSphereMesh;
    void RenderDebugSphere(
        const glm::vec3 &position, float radius, 
        const glm::vec3 &color
    );

    void SetupShaderValues();

    glm::mat4 JitterProjection(const glm::mat4& projection, const int frameIndex);

    GL::VertexArray *quadVAO;
    GL::Buffer<float, GL_ARRAY_BUFFER> *quadVBO;

    GL::BasicShaderProgram *lightPassShader;
    GL::BasicShaderProgram *postProcessShader;

    GL::FrameBuffer *geometryFramebuffer;
    GL::FrameBuffer *postProcessFramebuffer;
private:
    bool isWireframeMode = false;
    bool showFrustumColliders = false;
    EngineConfig::AntiAliasingMethod antiAliasingMethod;

    inline void GLDrawScreenQuad();

    inline void InitializeImGui(GLFWwindow* window);
    inline void GenerateScreenQuad();

    inline void SetupLightShader();
    inline void SetupPostProcessing();
    inline void SetupGeometryFramebuffer();

    MLAA_Components* GenerateMLAAComponents();
    TAA_Components* GenerateTAAComponents();
    SSAO_Components* GenerateSSAOComponents();

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
    TAA_Components *taa = nullptr;

    SSAO_Components *ssao = nullptr;
    GL::Texture noSSAOTexture;

    int windowWidth = 800;
    int windowHeight = 600;

    friend Input;
};