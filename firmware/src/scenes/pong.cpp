#include "Pong.h"
#include "App.h"
#include "GameState.h"
#include <M5Dial.h>

void Pong::begin() {
  redraw_ = true;

  playerScore_ = 0;
  cpuScore_ = 0;

  const int mid = (kPlayfieldTop + kPlayfieldBottom - kPaddleHeight) / 2;
  playerPaddleY_ = mid;
  cpuPaddleY_ = mid;

  lastMs_ = millis();
  tickAccumulatorMs_ = 0;

  M5Dial.Display.setTextDatum(middle_left);

  resetBall(+kBallSpeedX);
}

void Pong::resetBall(float servingVX) {
  ballX_ = 120 - kBallSize / 2;
  ballY_ = (kPlayfieldTop + kPlayfieldBottom) / 2 - kBallSize / 2;
  ballVX_ = servingVX;
  ballVY_ = random(-2, 3);
}

void Pong::update(const Input::InputState &in) {
  if (in.buttonDoublePressed) {
    App::requestScene(App::SceneId::Menu);
    return;
  }

  if (in.dialDelta != 0) {
    playerPaddleY_ += in.dialDelta * kPaddleSensitivity;
    if (playerPaddleY_ < kPlayfieldTop) {
      playerPaddleY_ = kPlayfieldTop;
    }
    if (playerPaddleY_ > kPlayfieldBottom - kPaddleHeight) {
      playerPaddleY_ = kPlayfieldBottom - kPaddleHeight;
    }
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

void Pong::stepPhysics() {
  const int leftPaddleX = kPaddleMargin;
  const int rightPaddleX = 240 - kPaddleMargin - kPaddleWidth;

  // CPU paddle tracks the ball with a limited step per tick so it's beatable.
  int cpuPaddleCenter = cpuPaddleY_ + kPaddleHeight / 2;
  int ballCenter = ballY_ + kBallSize / 2;
  if (ballCenter < cpuPaddleCenter - 2) {
    cpuPaddleY_ -= kCpuMaxStep;
  } else if (ballCenter > cpuPaddleCenter + 2) {
    cpuPaddleY_ += kCpuMaxStep;
  }
  if (cpuPaddleY_ < kPlayfieldTop) {
    cpuPaddleY_ = kPlayfieldTop;
  }
  if (cpuPaddleY_ > kPlayfieldBottom - kPaddleHeight) {
    cpuPaddleY_ = kPlayfieldBottom - kPaddleHeight;
  }

  ballX_ += ballVX_;
  ballY_ += ballVY_;

  if (ballY_ <= kPlayfieldTop) {
    ballY_ = kPlayfieldTop;
    ballVY_ = -ballVY_;
  }
  if (ballY_ + kBallSize >= kPlayfieldBottom) {
    ballY_ = kPlayfieldBottom - kBallSize;
    ballVY_ = -ballVY_;
  }

  // Player paddle (left) collision.
  if (ballVX_ < 0 && ballX_ <= leftPaddleX + kPaddleWidth &&
      ballX_ + kBallSize >= leftPaddleX &&
      ballY_ + kBallSize >= playerPaddleY_ &&
      ballY_ <= playerPaddleY_ + kPaddleHeight) {
    ballX_ = leftPaddleX + kPaddleWidth;
    ballVX_ = -ballVX_;
    float offset =
        (ballY_ + kBallSize / 2.0f) - (playerPaddleY_ + kPaddleHeight / 2.0f);
    ballVY_ = offset / 4.0f;
  }

  // CPU paddle (right) collision.
  if (ballVX_ > 0 && ballX_ + kBallSize >= rightPaddleX &&
      ballX_ <= rightPaddleX + kPaddleWidth &&
      ballY_ + kBallSize >= cpuPaddleY_ &&
      ballY_ <= cpuPaddleY_ + kPaddleHeight) {
    ballX_ = rightPaddleX - kBallSize;
    ballVX_ = -ballVX_;
    float offset =
        (ballY_ + kBallSize / 2.0f) - (cpuPaddleY_ + kPaddleHeight / 2.0f);
    ballVY_ = offset / 4.0f;
  }

  // Scoring: ball passed a paddle and left the playfield on that side.
  if (ballX_ + kBallSize < 0) {
    cpuScore_++;
    GameState::adjustHappiness(-2);
    resetBall(-kBallSpeedX);
  } else if (ballX_ > 240) {
    playerScore_++;
    GameState::adjustHappiness(+5);
    resetBall(+kBallSpeedX);
  }

  redraw_ = true;
}

void Pong::drawPaddle(int x, int y) const {
  M5Dial.Display.fillRect(x, y, kPaddleWidth, kPaddleHeight, TFT_WHITE);
}

void Pong::draw() {
  if (!redraw_) {
    return;
  }
  redraw_ = false;

  auto &d = M5Dial.Display;
  d.clear();

  d.setTextColor(TFT_WHITE);
  d.setTextSize(2);
  d.drawString("PONG", 100, 20);

  d.setTextSize(1.2);
  d.drawString(String(playerScore_) + "  -  " + String(cpuScore_), 100, 42);

  for (int lineY = kPlayfieldTop; lineY < kPlayfieldBottom; lineY += 12) {
    d.fillRect(118, lineY, 2, 6, TFT_DARKGREY);
  }

  drawPaddle(kPaddleMargin, (int)playerPaddleY_);
  drawPaddle(240 - kPaddleMargin - kPaddleWidth, cpuPaddleY_);

  d.fillRect((int)ballX_, (int)ballY_, kBallSize, kBallSize, TFT_CYAN);

  d.setTextSize(1);
  d.drawString("M5 x2 = QUIT", 45, 205);
}
