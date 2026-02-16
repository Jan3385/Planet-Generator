#pragma once

#include "Engine/Level.h"
#include "Engine/Renderer.h"
#include "Engine/Input.h"
#include "Engine/Lighting.h"
#include "Engine/Config.h"

class GameEngine{
public:
    static GameEngine* instance;
    static Renderer* renderer;
    static Level* currentLevel;
    static Input* input;
    static Lighting* lighting;

    float DeltaTime() const { return deltaTime; }

    GameEngine();
    ~GameEngine();

    void Run(const EngineConfig::Config& config);
private:
    void CalculateDeltaTime();
    double lastFrameTime = 0.0;
    float deltaTime = 0.166f;
    void InitializeGLFW(const EngineConfig::Config& config);
};