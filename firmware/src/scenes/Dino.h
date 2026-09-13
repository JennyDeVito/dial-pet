#pragma once
#include "Scene.h"
#include <cstdint>

class Dino : public Scene {
public:
  void begin() override;
  void update(const Input::InputState &in) override;
  void draw() override;

private:
  bool redraw_ = true;

  float dinoY_ = 0;
  float dinoVY_ = 0;
  bool onGround_ = true;
  bool wasTouching_ = false;

  float obstacleX_ = 0;
  bool hitThisObstacle_ = false;

  std::uint16_t passes_ = 0;

  std::uint32_t lastMs_ = 0;
  std::uint32_t tickAccumulatorMs_ = 0;

  // Ground/jump layout fills the round screen the same way Pong's playfield
  // sits between its title and its bottom hint line.
  static constexpr int kGroundY = 175;
  static constexpr int kDinoX = 40; // fixed horizontal position, same margin idea as Pong's paddles
  static constexpr int kDinoWidth = 18;
  static constexpr int kDinoHeight = 22;
  static constexpr int kObstacleWidth = 10;
  static constexpr int kObstacleHeight = 20;
  static constexpr float kObstacleSpeed = 2.5f; // px per physics tick, constant (no ramp-up)
  static constexpr float kGravity = 0.6f;
  static constexpr float kJumpVelocity = -7.5f;
  static constexpr std::uint32_t kTickMs = 16; // ~60 physics ticks/sec, same cadence as Pong

  void stepPhysics();
};
