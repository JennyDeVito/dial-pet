#pragma once

#include <cstdint>

namespace Input {
struct InputState {
  std::int32_t dialPosition;
  std::int32_t dialDelta;
  bool buttonPressed;
  bool buttonDoublePressed;
  bool touching;
  std::int16_t x;
  std::int16_t y;
};

void begin();

void update();

const InputState &state();
} // namespace Input