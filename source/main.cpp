#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"

void UpdateViewport(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char* argv[]) {
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Planet renderer", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, UpdateViewport);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // move later
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  

    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> vertexBuffer("VertexBuffer");
    vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(vertices[0]), GL_STATIC_DRAW);

    GL::VertexArray vertexVAO("VertexArray");

    vertexVAO.AddAttribute<glm::vec3>(0, 3, vertexBuffer, GL_FALSE, 0);
    vertexVAO.Unbind();

    GL::BasicShaderProgram shader("RenderVertex");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        vertexVAO.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}