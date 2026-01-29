#include "../scenes/Menu.h"
#include "GameState.h"
#include "TimeService.h"
#include <M5Dial.h>

static const char *kItems[] = {"PET", "DINO", "PONG", "ABOUT"};

void Menu::begin() {
  dialAccumulator_ = 0;
  selected_ = 0;
  lastSelected_ = selected_;
  redraw_ = true;
  popActive_ = false;

  M5Dial.Display.setTextSize(2);
  M5Dial.Display.setTextDatum(middle_left);
}

void Menu::moveSelection(std::int32_t delta) {
  if (delta == 0) {
    return;
  }

  selected_ += delta;

  if (selected_ < 0) {
    selected_ = kItemCount - 1;
  }

  if (selected_ >= kItemCount) {
    selected_ = 0;
  }

  if (selected_ != lastSelected_) {
    lastSelected_ = selected_;
    triggerPop();
  } else {
    redraw_ = true;
  }
}

void Menu::activateSelected() {
  switch (selected_) {
  case 0:
    App::requestScene(App::SceneId::Pet);
    break;
  case 1:
    App::requestScene(App::SceneId::Dino);
    break;
  case 2:
    App::requestScene(App::SceneId::Pong);
    break;
  case 3:
    App::requestScene(App::SceneId::Menu); // "About"
    break;
  }
}

int Menu::hitTestItem(std::int16_t x, std::int16_t y) const {
  const int startY = 70;
  const int stepY = 40;
  const int itemH = 32;

  for (int i = 0; i < kItemCount; i++) {
    int iy = startY + i * stepY;
    if (y >= (iy - itemH / 2) && y <= (iy + itemH / 2)) {
      return i;
    }
  }
  return -1;
}

void Menu::update(const Input::InputState &in) {
  dialAccumulator_ += in.dialDelta;

  const int step = 2; // adjust the speed

  while (dialAccumulator_ >= step) {
    dialAccumulator_ -= step;
    moveSelection(+1);
  }

  while (dialAccumulator_ <= -step) {
    dialAccumulator_ += step;
    moveSelection(-1);
  }

  if (in.buttonPressed) {
    activateSelected();
  }

  if (in.touching) {
    int hit = hitTestItem(in.x, in.y);

    if (hit >= 0 && hit < kItemCount) {
      if (hit != selected_) {
        selected_ = hit;
        lastSelected_ = selected_;

        triggerPop();
      } else {
        redraw_ = true;
      }
      activateSelected();
    }
  }
}

void Menu::draw() {
  std::uint32_t now = millis();
  bool popStillRunning = popActive_ && (now - popStartMs_ < kPopDurationMs);

  if (!redraw_ && !popStillRunning) {
    return;
  }
  redraw_ = false;

  auto &display = M5Dial.Display;

  display.clear();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.drawString("DIAL PET", 70, 30);

  const int startY = 70;
  const int stepY = 35;

  display.setTextSize(1.5);

  for (int i = 0; i < kItemCount; i++) {
    int y = startY + i * stepY;
    if (i == selected_) {
      display.fillRoundRect(30, y - 16, 70, 30, 7);
      display.setTextColor(TFT_BLACK);
    } else {
      display.setTextColor(TFT_WHITE);
    }
    display.drawString(kItems[i], 43, y);
  }

  drawPreview();

  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  display.drawString("BUTTON = SELECT", 80, 205);
  display.drawString("DIAL = MOVE", 92, 220);

  /*
   * // DEBUG STARTS - it will collapse with the menu preview
  const auto &in = Input::state();
  display.setTextColor(TFT_RED);
  display.drawString("pos: " + String(in.dialPosition), 120, 130);
  display.drawString("delta: " + String(in.dialDelta), 120, 150);
  display.drawString("ac: " + String(dialAccumulator_), 120, 170);
  * // DEBUG ENDS
  */

  display.setTextColor(TFT_WHITE);

  drawPop();

  now = millis();
  popStillRunning = popActive_ && (now - popStartMs_ < kPopDurationMs);

  if (popStillRunning) {
    redraw_ = true;
  } else {
    popActive_ = false;
  }
}

void Menu::triggerPop() {
  popStartMs_ = millis();
  popActive_ = true;
  redraw_ = true;
}

void Menu::drawPreview() const {
  auto &display = M5Dial.Display;
  const auto &pet = GameState::pet();

  const int x = 120;
  int y0 = 60;
  const int line = 18;

  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  display.drawString(
      String("Time: ") + (TimeService::synced() ? "sync" : "..."), x + 10, 55);

  display.setTextSize(1.2);

  switch (selected_) {

  case 0: {
    display.drawString("VIRTUAL PET", x, y0 + line);
    display.setTextSize(1);
    display.drawString("Feed | Clean | Play", x, y0 + 2 * line);
    display.setTextSize(1.2);
    display.drawString("Hunger:" + String(pet.hunger), x, y0 + 3 * line);
    display.drawString("Sleep:" + String(pet.sleepiness), x, y0 + 4 * line);
  } break;

  case 1: {
    y0 = 80;
    display.drawString("DINO GAME", x, y0 + line);
    display.drawString("Press to jump", x, y0 + 2 * line);
    display.drawString("High score:", x, y0 + 3 * line);
    display.drawString("--", x, y0 + 4 * line);
  } break;

  case 2: {
    y0 = 100;
    display.drawString("CLASSIC PONG", x, y0 + line);
    display.drawString("Spin the dial", x, y0 + 2 * line);
    display.drawString("1P VS CPU", x, y0 + 3 * line);
  } break;

  case 3: {
    y0 = 120;
    display.drawString("DEVELOPED BY", x, y0 + line);
    display.drawString("Jenny DeVito", x, y0 + 2 * line);
    display.drawString("WorstMakers", x, y0 + 3 * line);
  } break;
  }
}

void Menu::drawPop() const {
  if (!popActive_) {
    return;
  }

  auto &display = M5Dial.Display;
  std::uint32_t now = millis();
  std::uint32_t elapsed = now - popStartMs_;

  if (elapsed >= kPopDurationMs) {
    return;
  }

  float t = (float)elapsed / (float)kPopDurationMs;
  int r = 8 + (int)(t * 35);

  // BUG: draws the circle in the center of the screen
  // display.drawCircle(120, 120, r);
  // display.drawCircle(120, 120, r - 1);

  // FIX: draws the circle inside the rectangles
  const int startY = 70;
  const int stepY = 35;
  const int y = startY + selected_ * stepY;
  const int x = 65;

  display.drawCircle(x, y, r, TFT_CYAN);
  display.drawCircle(x, y, r - 1, TFT_CYAN);

  if (selected_ < 0 || selected_ > kItemCount) {
    return;
  }
}