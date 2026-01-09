#pragma once

#include "Engine/Level.h"
#include "Engine/Renderer.h"

class GameEngine{
public:
    static GameEngine* instance;
    Renderer* renderer;
    Level* currentLevel;

    GameEngine();
    ~GameEngine();

    void Run();
};