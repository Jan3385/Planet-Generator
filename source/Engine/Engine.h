#pragma once

#include "Engine/Level.h"
#include "Engine/Renderer.h"
#include "Engine/Input.h"
#include "Engine/Lighting.h"

class GameEngine{
public:
    static GameEngine* instance;
    static Renderer* renderer;
    static Level* currentLevel;
    static Input* input;
    static Lighting* lighting;

    GameEngine();
    ~GameEngine();

    void Run();
private:
    void InitializeGLFW();
};