#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Engine/Level.h"

#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"
#include "Object/BaseObject.h"

#include "Component/Essential/RenderComponent.h"
#include "Engine/Engine.h"

int main(int argc, char* argv[]) {
    GameEngine engine;
    engine.Run();

    return EXIT_SUCCESS;
}