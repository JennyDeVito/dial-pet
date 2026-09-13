#pragma once

#include <cstdint>

namespace GameState {
struct PetState {
  std::uint8_t hunger;     // 0 = full; 100 = starving
  std::uint8_t sleepiness; // 0 = rest; 100 = tired
  std::uint8_t happiness;  // 0 = sad; 100 = happy
};

void begin(std::uint32_t nowMs);
void tick(std::uint32_t nowMs);
const PetState &pet();

void feed(std::uint8_t hungerAmount, std::uint8_t happinessAmount);
void rest(std::uint8_t amount);
void play(std::uint8_t amount);
void adjustHappiness(int delta);
} // namespace GameState