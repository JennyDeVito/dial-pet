#pragma once
#include <cstdint>

namespace App {
enum class SceneId : std::uint8_t { Menu, Pet, Dino, Pong };
void begin();
void loop();
void requestScene(SceneId id);
}