#include "GameState.h"

namespace GameState {
static PetState pet_;
static std::uint32_t lastMs_ = 0;
static std::uint32_t hungerAccumulatorMs_ = 0; // acc to not depend on loop rate
static std::uint32_t sleepAccumulatorMs_ = 0;  // acc to not depend on loop rate
static constexpr std::uint32_t kHungerStepMs = 12000; // +1 every 12"
static constexpr std::uint32_t kSleepStepMs = 18000;  // +1 every 18"

static std::uint8_t clamp100(int v) {
  if (v < 0) {
    return 0;
  }
  if (v > 100) {
    return 100;
  }
  return static_cast<std::uint8_t>(v);
}

void begin(std::uint32_t nowMs) {
  pet_ = {50, 50}; // start in the middle
  lastMs_ = nowMs;
  hungerAccumulatorMs_ = 0;
  sleepAccumulatorMs_ = 0;
}

const PetState &pet() { return pet_; }

void tick(std::uint32_t nowMs) {
  if (lastMs_ == 0) {
    begin(nowMs);
    return;
  }
  std::uint32_t deltaTime = nowMs - lastMs_;
  lastMs_ = nowMs;
  hungerAccumulatorMs_ += deltaTime;
  while (hungerAccumulatorMs_ >= kHungerStepMs) {
    hungerAccumulatorMs_ -= kHungerStepMs;
    pet_.hunger = clamp100(pet_.hunger + 1);
  }
  sleepAccumulatorMs_ += deltaTime;
  while (sleepAccumulatorMs_ >= kSleepStepMs) {
    sleepAccumulatorMs_ -= kSleepStepMs;
    pet_.sleepiness = clamp100(pet_.sleepiness + 1);
  }
}
} // namespace GameState