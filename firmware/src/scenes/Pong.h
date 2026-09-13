#pragma once
#include "Scene.h"
#include <cstdint>

class Pong : public Scene {
public:
  void begin() override;
  void update(const Input::InputState &in) override;
  void draw() override;

private:
  bool redraw_ = true;

  float playerPaddleY_ = 0;
  int cpuPaddleY_ = 0;
  float ballX_ = 0;
  float ballY_ = 0;
  float ballVX_ = 0;
  float ballVY_ = 0;

  std::uint16_t playerScore_ = 0;
  std::uint16_t cpuScore_ = 0;

  std::uint32_t lastMs_ = 0;
  std::uint32_t tickAccumulatorMs_ = 0;

  // Playfield fills the round screen the same way the Pet scene's bars sit
  // between its "PET" title and its bottom hint line.
  static constexpr int kPlayfieldTop = 60;
  static constexpr int kPlayfieldBottom = 195;
  static constexpr int kPaddleWidth = 6;
  static constexpr int kPaddleHeight = 34;
  static constexpr int kPaddleMargin = 35; // distance from the screen's left/right edge
  static constexpr int kBallSize = 6;
  static constexpr float kPaddleSensitivity = 6.0f;
  static constexpr int kCpuMaxStep = 2; // px per physics tick
  // Baseline was 3 px/tick; requested 30% slower.
  static constexpr float kBallSpeedX = 2.1f;
  static constexpr std::uint32_t kTickMs = 16; // ~60 physics ticks/sec

  void resetBall(float servingVX);
  void stepPhysics();
  void drawPaddle(int x, int y) const;
};
