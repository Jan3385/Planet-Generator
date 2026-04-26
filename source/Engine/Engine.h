#pragma once

#include "Engine/Level.h"
#include "Engine/Renderer.h"
#include "Engine/Input.h"
#include "Engine/Lighting.h"
#include "Engine/Physics.h"
#include "Engine/Config.h"
#include "Object/Material/MaterialLibrary.h"

#define FPS_HISTORY_SIZE 100

class GameEngine{
public:
    static constexpr float FIXED_UPDATE_INTERVAL = 1.0f / 60.0f; // 60 FPS fixed update

    static GameEngine* instance;
    static Renderer* renderer;
    static Level* currentLevel;
    static Input* input;
    static Lighting* lighting;
    static Physics* physics;
    static MaterialLibrary* materialLibrary;

    GameEngine();
    ~GameEngine();

    void Run(EngineConfig::Config& config);

    float DeltaTime() const { return deltaTime; }
    static constexpr float FixedDeltaTime() { return FIXED_UPDATE_INTERVAL; }
    float GetFPS() const { return this->FPS; }
    float GetAvgFPS() const { return this->avgFPS; }
private:
    void CalculateDeltaTime();
    double lastFrameTime = 0.0;
    float deltaTime = 0.166f;
    float FPS = 60.0f;
    float avgFPS = 60.0f;
    std::deque<float> fpsHistory = std::deque<float>(1, 60.0f);

    void InitializeGLFW(const EngineConfig::Config& config);
};