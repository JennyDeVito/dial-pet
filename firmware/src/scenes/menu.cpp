#include "../scenes/Menu.h"
#include <M5Dial.h>

static const char *kItems[] = {"PET", "DINO", "PONG", "ABOUT"};

void Menu::begin() {
  dialAccumulator_ = 0;
  selected_ = 0;
  redraw_ = true;

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
  redraw_ = true;
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
        redraw_ = true;
      }
      activateSelected();
    }
  }
}

void Menu::draw() {
  if (!redraw_) {
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
      display.fillRoundRect(35, y - 16, 75, 30, 8);
      display.setTextColor(TFT_BLACK);
    } else {
      display.setTextColor(TFT_WHITE);
    }
    display.drawString(kItems[i], 50, y);
  }
  display.setTextColor(TFT_WHITE);
  display.setTextSize(1);
  display.drawString("BUTTON = SELECT", 70, 205);
  display.drawString("DIAL = MOVE", 80, 220);
  /*
   * // DEBUG
   */
  const auto &in = Input::state();
  display.setTextColor(TFT_RED);
  display.drawString("pos: " + String(in.dialPosition), 120, 130);
  display.drawString("delta: " + String(in.dialDelta), 120, 150);
  display.drawString("ac: " + String(dialAccumulator_), 120, 170);
  display.setTextColor(TFT_WHITE);
}