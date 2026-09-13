#include "Dino.h"
#include "App.h"
#include "GameState.h"
#include <M5Dial.h>

void Dino::begin() {
  redraw_ = true;

  passes_ = 0;

  dinoY_ = kGroundY - kDinoHeight;
  dinoVY_ = 0;
  onGround_ = true;
  wasTouching_ = false;

  obstacleX_ = 240;
  hitThisObstacle_ = false;

  lastMs_ = millis();
  tickAccumulatorMs_ = 0;

  M5Dial.Display.setTextDatum(middle_left);
}

void Dino::update(const Input::InputState &in) {
  if (in.buttonDoublePressed) {
    App::requestScene(App::SceneId::Menu);
    return;
  }

  bool touchEdge = in.touching && !wasTouching_;
  wasTouching_ = in.touching;
  if (touchEdge && onGround_) {
    onGround_ = false;
    dinoVY_ = kJumpVelocity;
    redraw_ = true;
  }

  std::uint32_t now = millis();
  std::uint32_t delta = now - lastMs_;
  lastMs_ = now;
  tickAccumulatorMs_ += delta;

  while (tickAccumulatorMs_ >= kTickMs) {
    tickAccumulatorMs_ -= kTickMs;
    stepPhysics();
  }
}

void Dino::stepPhysics() {
  if (!onGround_) {
    dinoVY_ += kGravity;
    dinoY_ += dinoVY_;
    if (dinoY_ >= kGroundY - kDinoHeight) {
      dinoY_ = kGroundY - kDinoHeight;
      dinoVY_ = 0;
      onGround_ = true;
    }
  }

  obstacleX_ -= kObstacleSpeed;

  // Collision: obstacle overlaps the dino's fixed x-slot and the dino hasn't
  // jumped high enough to clear it.
  bool overlapX =
      obstacleX_ < kDinoX + kDinoWidth && obstacleX_ + kObstacleWidth > kDinoX;
  bool overlapY = dinoY_ + kDinoHeight > kGroundY - kObstacleHeight;
  if (!hitThisObstacle_ && overlapX && overlapY) {
    hitThisObstacle_ = true;
    GameState::adjustHappiness(-2);
  }

  // Obstacle fully passed: score a clear (unless it already hit), then
  // respawn it off the right edge with a randomized gap.
  if (obstacleX_ + kObstacleWidth < 0) {
    if (!hitThisObstacle_) {
      passes_++;
      GameState::adjustHappiness(+5);
    }
    obstacleX_ = 240 + random(0, 60);
    hitThisObstacle_ = false;
  }

  redraw_ = true;
}

void Dino::draw() {
  if (!redraw_) {
    return;
  }
  redraw_ = false;

  auto &d = M5Dial.Display;
  d.clear();

  d.setTextColor(TFT_WHITE);
  d.setTextSize(2);
  d.drawString("DINO", 100, 20);

  d.setTextSize(1.2);
  d.drawString("PASSES: " + String(passes_), 100, 42);

  d.drawFastHLine(0, kGroundY, 240, TFT_DARKGREY);

  d.fillRect(kDinoX, (int)dinoY_, kDinoWidth, kDinoHeight, TFT_WHITE);
  d.fillRect((int)obstacleX_, kGroundY - kObstacleHeight, kObstacleWidth,
             kObstacleHeight, TFT_CYAN);

  d.setTextSize(1);
  d.drawString("M5 x2 = QUIT", 92, 220);
}
