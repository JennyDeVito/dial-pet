#include "Pet.h"
#include "App.h"
#include "GameState.h"
#include <M5Dial.h>

static const char *kActionNames[] = {"FEED", "SLEEP", "PLAY"};

struct FoodInfo {
  const char *name;
  std::uint8_t hungerRelief;
  std::uint8_t happinessBoost;
};

static const FoodInfo kFoods[] = {
    {"BURGER", 10, 0},
    {"MILKSHAKE", 6, 8},
    {"FRUIT", 4, 2},
    {"STEAK", 18, 0},
};

static const char *kGameNames[] = {"PONG", "DINO"};

void Pet::begin() {
  dialAccumulator_ = 0;
  selected_ = Action::Feed;
  selectedFood_ = Food::Burger;
  selectedGame_ = Game::Pong;
  mode_ = Mode::ActionMenu;
  redraw_ = true;
  toast_ = nullptr;
  toastStartMs_ = 0;

  sleepLastMs_ = 0;
  sleepTickAccumulatorMs_ = 0;
  sleepAnimStartMs_ = 0;
  lastSleepAnimMs_ = 0;

  M5Dial.Display.setTextDatum(middle_left);
}

void Pet::moveAction(int delta) {
  if (delta == 0) {
    return;
  }

  int idx = (int)selected_;
  idx += delta;

  const int count = (int)Action::Count;
  if (idx < 0) {
    idx = count - 1;
  }
  if (idx >= count) {
    idx = 0;
  }

  selected_ = (Action)idx;

  redraw_ = true;
}

void Pet::moveFood(int delta) {
  if (delta == 0) {
    return;
  }

  int idx = (int)selectedFood_;
  idx += delta;

  const int count = (int)Food::Count;
  if (idx < 0) {
    idx = count - 1;
  }
  if (idx >= count) {
    idx = 0;
  }

  selectedFood_ = (Food)idx;

  redraw_ = true;
}

void Pet::moveGame(int delta) {
  if (delta == 0) {
    return;
  }

  int idx = (int)selectedGame_;
  idx += delta;

  const int count = (int)Game::Count;
  if (idx < 0) {
    idx = count - 1;
  }
  if (idx >= count) {
    idx = 0;
  }

  selectedGame_ = (Game)idx;

  redraw_ = true;
}

void Pet::eatSelectedFood() {
  const FoodInfo &food = kFoods[(int)selectedFood_];
  GameState::feed(food.hungerRelief, food.happinessBoost);

  toast_ = food.name;
  toastStartMs_ = millis();

  mode_ = Mode::ActionMenu;
  dialAccumulator_ = 0;
  redraw_ = true;
}

void Pet::doAction() {
  switch (selected_) {
  case Action::Feed: {
    mode_ = Mode::FoodSelect;
    selectedFood_ = Food::Burger;
    dialAccumulator_ = 0;
    redraw_ = true;
    return;
  }
  case Action::Sleep: {
    mode_ = Mode::Sleeping;
    sleepLastMs_ = millis();
    sleepTickAccumulatorMs_ = 0;
    sleepAnimStartMs_ = millis();
    lastSleepAnimMs_ = millis();
    redraw_ = true;
    return;
  }
  case Action::Play: {
    GameState::play(8);
    mode_ = Mode::GameSelect;
    selectedGame_ = Game::Pong;
    dialAccumulator_ = 0;
    redraw_ = true;
    return;
  }
  default:
    break;
  }

  toastStartMs_ = millis();

  redraw_ = true;
}

void Pet::launchSelectedGame() {
  switch (selectedGame_) {
  case Game::Pong: {
    App::requestScene(App::SceneId::Pong);
    break;
  }
  case Game::Dino: {
    App::requestScene(App::SceneId::Dino);
    break;
  }
  default:
    break;
  }
}

void Pet::wakeUp(const char *toastMsg) {
  mode_ = Mode::ActionMenu;
  dialAccumulator_ = 0;
  toast_ = toastMsg;
  toastStartMs_ = millis();
  redraw_ = true;
}

void Pet::update(const Input::InputState &in) {
  if (mode_ != Mode::Sleeping) {
    dialAccumulator_ += in.dialDelta;
    const int step = 2;

    while (dialAccumulator_ >= step) {
      dialAccumulator_ -= step;
      if (mode_ == Mode::FoodSelect) {
        moveFood(+1);
      } else if (mode_ == Mode::GameSelect) {
        moveGame(+1);
      } else {
        moveAction(+1);
      }
    }

    while (dialAccumulator_ <= -step) {
      dialAccumulator_ += step;
      if (mode_ == Mode::FoodSelect) {
        moveFood(-1);
      } else if (mode_ == Mode::GameSelect) {
        moveGame(-1);
      } else {
        moveAction(-1);
      }
    }
  }

  if (in.buttonDoublePressed) {
    if (mode_ == Mode::FoodSelect || mode_ == Mode::GameSelect) {
      mode_ = Mode::ActionMenu;
      dialAccumulator_ = 0;
      redraw_ = true;
    } else if (mode_ == Mode::ActionMenu) {
      App::requestScene(App::SceneId::Menu);
    }
    // Sleeping: double-press does nothing - only the wake button (or the
    // sleep bar emptying) can end a sleeping session.
    return;
  }

  if (in.buttonPressed) {
    if (mode_ == Mode::FoodSelect) {
      eatSelectedFood();
    } else if (mode_ == Mode::GameSelect) {
      launchSelectedGame();
    } else if (mode_ == Mode::Sleeping) {
      wakeUp("Woke up!");
    } else {
      doAction();
    }
  }

  if (mode_ == Mode::Sleeping) {
    std::uint32_t now = millis();
    std::uint32_t delta = now - sleepLastMs_;
    sleepLastMs_ = now;
    sleepTickAccumulatorMs_ += delta;

    while (sleepTickAccumulatorMs_ >= kSleepTickMs &&
           GameState::pet().sleepiness > 0) {
      sleepTickAccumulatorMs_ -= kSleepTickMs;
      GameState::rest(1);
    }

    if (GameState::pet().sleepiness == 0) {
      wakeUp(nullptr);
    } else if (now - lastSleepAnimMs_ > kSleepAnimFrameMs) {
      lastSleepAnimMs_ = now;
      redraw_ = true;
    }
  }

  if (toast_ && (millis() - toastStartMs_ > kToastMs)) {
    toast_ = nullptr;
    redraw_ = true;
  }

  static std::uint32_t lastUiMs = 0;
  std::uint32_t now = millis();
  if (now - lastUiMs > 1000) {
    lastUiMs = now;
    redraw_ = true;
  }
}

void Pet::drawBar(int x, int y, int w, int h, std::uint8_t value) const {
  auto &d = M5Dial.Display;
  d.drawRect(x, y, w, h);

  int fill = (w - 2) * (int)value / 100;
  if (fill < 0) {
    fill = 0;
  }
  if (fill > w - 2) {
    fill = w - 2;
  }

  d.fillRect(x + 1, y + 1, fill, h - 2);
}

void Pet::drawActionMenu() const {
  auto &d = M5Dial.Display;
  const int y = 160;
  const int x = 95;

  d.setTextSize(1.5);
  d.drawString("ACTION:", x - 80, y);

  d.fillRoundRect(x - 10, y - 14, 70, 26, 7);
  d.setTextColor(TFT_BLACK);
  d.drawString(kActionNames[(int)selected_], x, y);
  d.setTextColor(TFT_WHITE);

  d.setTextSize(1);
  d.drawString("M5 x1 = GO | M5 x2 = BACK", 45, 205);
}

void Pet::drawFoodIcon(Food food, int cx, int cy) const {
  auto &d = M5Dial.Display;

  switch (food) {
  case Food::Burger: {
    d.fillRoundRect(cx - 22, cy - 16, 44, 16, 8, TFT_ORANGE);
    d.fillRect(cx - 22, cy - 2, 44, 4, TFT_GREEN);
    d.fillRect(cx - 22, cy + 2, 44, 6, TFT_BROWN);
    d.fillRoundRect(cx - 22, cy + 8, 44, 10, 5, TFT_ORANGE);
    break;
  }
  case Food::Milkshake: {
    d.fillRect(cx + 4, cy - 26, 3, 20, TFT_RED);
    d.fillEllipse(cx, cy - 10, 12, 6, TFT_WHITE);
    d.fillRoundRect(cx - 10, cy - 4, 20, 24, 3, TFT_PINK);
    break;
  }
  case Food::Fruit: {
    d.fillRect(cx - 1, cy - 14, 3, 8, TFT_BROWN);
    d.fillEllipse(cx + 6, cy - 12, 6, 3, TFT_GREEN);
    d.fillCircle(cx, cy + 4, 14, TFT_RED);
    break;
  }
  case Food::Steak: {
    d.fillRoundRect(cx - 20, cy - 12, 40, 24, 10, TFT_BROWN);
    d.drawLine(cx - 12, cy - 10, cx - 4, cy + 10, TFT_BLACK);
    d.drawLine(cx - 2, cy - 10, cx + 6, cy + 10, TFT_BLACK);
    d.drawLine(cx + 8, cy - 10, cx + 16, cy + 10, TFT_BLACK);
    break;
  }
  default:
    break;
  }
}

void Pet::drawFoodSelect() const {
  auto &d = M5Dial.Display;
  const FoodInfo &food = kFoods[(int)selectedFood_];

  const int cx = 120;
  const int cy = 155;

  drawFoodIcon(selectedFood_, cx, cy);

  d.setTextDatum(middle_center);
  d.setTextSize(1.2);
  d.drawString(food.name, cx, cy + 28);
  d.setTextDatum(middle_left);

  d.setTextSize(1);
  d.drawString("M5 x1 = EAT | M5 x2 = CANCEL", 40, 205);
}

void Pet::drawSleeping() const {
  auto &d = M5Dial.Display;

  const int cx = 120;
  const int cy = 170;

  // Slow breathing pulse: a triangle-wave ramp over a 2s cycle, 0..1000..0.
  std::uint32_t cycle = (millis() - sleepAnimStartMs_) % 2000;
  int breathe = (int)(cycle < 1000 ? cycle : (2000 - cycle));
  int bodyRx = 34 + (breathe * 4) / 1000;
  int bodyRy = 22 + (breathe * 3) / 1000;

  // Curled-up fluffy body.
  d.fillEllipse(cx, cy, bodyRx, bodyRy, TFT_ORANGE);
  d.fillEllipse(cx - 6, cy + 6, bodyRx / 2, bodyRy / 2, TFT_WHITE);

  // Curled tail resting along the body.
  d.fillEllipse(cx + 26, cy + 4, 12, 6, TFT_ORANGE);

  // Round, flat-faced Persian head.
  d.fillCircle(cx - 22, cy - 10, 16, TFT_ORANGE);

  // Ears.
  d.fillTriangle(cx - 34, cy - 18, cx - 28, cy - 30, cx - 20, cy - 20,
                 TFT_ORANGE);
  d.fillTriangle(cx - 16, cy - 20, cx - 10, cy - 30, cx - 4, cy - 18,
                 TFT_ORANGE);

  // Closed sleepy eyes.
  d.drawLine(cx - 30, cy - 10, cx - 24, cy - 8, TFT_BLACK);
  d.drawLine(cx - 18, cy - 8, cx - 12, cy - 10, TFT_BLACK);

  // Nose and whiskers.
  d.fillTriangle(cx - 23, cy - 4, cx - 21, cy - 4, cx - 22, cy - 2, TFT_PINK);
  d.drawLine(cx - 36, cy - 6, cx - 46, cy - 8, TFT_WHITE);
  d.drawLine(cx - 36, cy - 3, cx - 46, cy - 3, TFT_WHITE);
  d.drawLine(cx - 8, cy - 6, cx + 2, cy - 8, TFT_WHITE);
  d.drawLine(cx - 8, cy - 3, cx + 2, cy - 3, TFT_WHITE);

  // Floating "Zzz" that grows and cycles every 500ms.
  static const char *kZGlyphs[] = {"Z", "Zz", "Zzz"};
  int zStep = (int)((millis() / 500) % 3);
  d.setTextSize(1.4);
  d.setTextColor(TFT_WHITE);
  d.drawString(kZGlyphs[zStep], cx + 18, cy - 34);

  d.setTextSize(1);
  d.drawString("M5 x1 = WAKE UP", cx - 43, 213);
}

void Pet::drawGameIcon(Game game, int cx, int cy) const {
  auto &d = M5Dial.Display;

  switch (game) {
  case Game::Pong: {
    for (int lineY = cy - 26; lineY < cy + 26; lineY += 10) {
      d.fillRect(cx - 1, lineY, 2, 6, TFT_DARKGREY);
    }
    d.fillRect(cx - 30, cy - 12, 5, 24, TFT_WHITE);
    d.fillRect(cx + 25, cy - 12, 5, 24, TFT_WHITE);
    d.fillRect(cx - 3, cy - 3, 6, 6, TFT_CYAN);
    break;
  }
  case Game::Dino: {
    d.fillRect(cx - 16, cy - 8, 24, 18, TFT_DARKGREY);
    d.fillRect(cx + 2, cy - 22, 14, 16, TFT_DARKGREY);
    d.fillRect(cx - 16, cy + 10, 6, 10, TFT_DARKGREY);
    d.fillRect(cx - 4, cy + 10, 6, 10, TFT_DARKGREY);
    d.fillRect(cx + 12, cy - 18, 2, 2, TFT_BLACK);
    break;
  }
  default:
    break;
  }
}

void Pet::drawGameSelect() const {
  auto &d = M5Dial.Display;

  const int cx = 120;
  const int cy = 155;

  drawGameIcon(selectedGame_, cx, cy);

  d.setTextDatum(middle_center);
  d.setTextSize(1.2);
  d.drawString(kGameNames[(int)selectedGame_], cx, cy + 28);
  d.setTextDatum(middle_left);

  d.setTextSize(1);
  d.drawString("M5 x1 = SELECT | M5 x2 = BACK", 45, 205);
}

void Pet::draw() {
  if (!redraw_) {
    return;
  }
  redraw_ = false;

  auto &d = M5Dial.Display;
  d.clear();

  d.setTextColor(TFT_WHITE);
  d.setTextSize(2);
  d.drawString("PET", 100, 20); // confirm screen positioning

  const auto &pet = GameState::pet();
  d.setTextSize(1.2);

  d.drawString("SLEEP", 34, 55);
  drawBar(77, 50, 120, 12, pet.sleepiness);

  d.drawString("HUNGER", 25, 80);
  drawBar(75, 75, 120, 12, pet.hunger);

  d.drawString("HAPPINESS", 15, 105);
  drawBar(86, 100, 120, 12, pet.happiness);

  if (mode_ == Mode::FoodSelect) {
    drawFoodSelect();
  } else if (mode_ == Mode::Sleeping) {
    drawSleeping();
  } else if (mode_ == Mode::GameSelect) {
    drawGameSelect();
  } else {
    drawActionMenu();
  }

  if (toast_) {
    d.setTextSize(1.5);
    d.drawString(toast_, 150, 190);
  }
}
