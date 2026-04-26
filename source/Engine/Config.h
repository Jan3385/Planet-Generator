#pragma once

#include <cstdint>

namespace EngineConfig{
enum class AntiAliasingMethod {
    None,   // No anti-aliasing
    FXAA,   // Fast Approximate Anti-Aliasing
    MLAA,   // Morphological Anti-Aliasing
    TAA     // Temporal Anti-Aliasing
};
enum class WindowMode {
    Windowed,
    Borderless,
    Fullscreen
};

struct Config{
    WindowMode windowMode = WindowMode::Windowed;

    uint16_t windowWidth = 800;     // Used only in Windowed mode
    uint16_t windowHeight = 600;    // Used only in Windowed mode

    AntiAliasingMethod antiAliasingMethod = AntiAliasingMethod::None;
    float gamma = 2.2f;
    bool VSync = true;
};
}
