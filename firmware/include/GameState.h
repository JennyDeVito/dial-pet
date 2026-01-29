#pragma once

#include <cstdint>

namespace GameState {
struct PetState {
  std::uint8_t hunger;     // 0 = full; 100 = starving
  std::uint8_t sleepiness; // 0 = rest; 100 = tired
};
void begin(std::uint32_t nowMs);
void tick(std::uint32_t nowMs);
const PetState &pet();
} // namespace GameState