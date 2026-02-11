#include <iostream>

#include "Engine/Engine.h"

int main(int argc, char* argv[]) {
    Debug::Logger::Instance().minLogLevel = Debug::Logger::Level::SPAM;

    GameEngine engine;
    GameEngine::Config config;

    engine.Run(config);

    return EXIT_SUCCESS;
}