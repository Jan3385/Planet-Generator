#pragma once

#include <cstdint>

namespace EngineConfig{
enum class AntiAliasingMethod {
    None,   // No anti-aliasing
    FXAA,   // Fast Approximate Anti-Aliasing
    SMAA,   // Subpixel Morphological Anti-Aliasing
    TAA     // Temporal Anti-Aliasing
};

struct Config{
    uint16_t windowWidth = 800;
    uint16_t windowHeight = 600;

    AntiAliasingMethod antiAliasingMethod = AntiAliasingMethod::None;
    float gamma = 2.2f;
    bool VSync = true;
};
}
