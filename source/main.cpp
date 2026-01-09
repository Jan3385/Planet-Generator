#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"

#include "Object/BaseObject.h"

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
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    }; 

    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> vertexBuffer("VertexBuffer");
    vertexBuffer.SetData(vertices, sizeof(vertices) / sizeof(vertices[0]), GL_STATIC_DRAW);
    GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer> indexBuffer("IndexBuffer");
    indexBuffer.SetData(indices, sizeof(indices) / sizeof(indices[0]), GL_STATIC_DRAW);

    GL::VertexArray vertexVAO("VertexArray");

    vertexVAO.AddAttribute<glm::vec3>(0, 3, vertexBuffer, GL_FALSE, 0);
    vertexVAO.Unbind();

    GL::BasicShaderProgram shader("RenderVertex");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        vertexVAO.Bind();
        indexBuffer.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}