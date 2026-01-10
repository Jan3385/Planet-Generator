#pragma once

#include "Engine/Level.h"
#include "Engine/Renderer.h"
#include "Engine/Input.h"

class GameEngine{
public:
    static GameEngine* instance;
    static Renderer* renderer;
    static Level* currentLevel;
    static Input* input;

    GameEngine();
    ~GameEngine();

    void Run();
private:
    void InitializeGLFW();
};