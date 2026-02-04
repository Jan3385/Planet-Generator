#pragma once

#include "Engine/Level.h"
#include "Engine/Renderer.h"
#include "Engine/Input.h"
#include "Engine/Lighting.h"

class GameEngine{
public:
    struct Config{
        uint16_t windowWidth = 800;
        uint16_t windowHeight = 600;

        uint8_t MSAA_Samples = 8; // 1 means no MSAA
        float gamma = 2.2f;
        bool VSync = true;
    };

    static GameEngine* instance;
    static Renderer* renderer;
    static Level* currentLevel;
    static Input* input;
    static Lighting* lighting;

    float DeltaTime() const { return deltaTime; }

    GameEngine();
    ~GameEngine();

    void Run(const Config& config);
private:
    void CalculateDeltaTime();
    double lastFrameTime = 0.0;
    float deltaTime = 0.166f;
    void InitializeGLFW(const Config& config);
};