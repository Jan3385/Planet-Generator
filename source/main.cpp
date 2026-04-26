#include <iostream>

#include "Engine/Engine.h"

int main(int argc, char* argv[]) {
    Debug::Logger::Instance().minLogLevel = Debug::Logger::Level::SPAM;

    GameEngine engine;
    EngineConfig::Config config;
    config.antiAliasingMethod = EngineConfig::AntiAliasingMethod::TAA;
    config.windowMode = EngineConfig::WindowMode::Windowed;

    engine.Run(config);

    return EXIT_SUCCESS;
}