#pragma once
#include "Scene.h"
#include <cstdint>

class Pet : public Scene {
public:
  void begin() override;
  void update(const Input::InputState &in) override;
  void draw() override;

private:
  enum class Action : std::uint8_t { Feed, Sleep, Play, Count };
  enum class Food : std::uint8_t { Burger, Milkshake, Fruit, Steak, Count };
  enum class Game : std::uint8_t { Pong, Dino, Count };
  enum class Mode : std::uint8_t { ActionMenu, FoodSelect, Sleeping, GameSelect };

  std::int32_t dialAccumulator_ = 0;
  Action selected_ = Action::Feed;
  Food selectedFood_ = Food::Burger;
  Game selectedGame_ = Game::Pong;
  Mode mode_ = Mode::ActionMenu;

  bool redraw_ = true;

  const char *toast_ = nullptr;
  std::uint32_t toastStartMs_ = 0;
  static constexpr std::uint32_t kToastMs = 900;

  // Sleeping session: sleepiness drains by 1 every kSleepTickMs while asleep
  // (~600ms/point => a full 100->0 sleep takes about a minute), and the
  // animation is redrawn every kSleepAnimFrameMs regardless of input.
  std::uint32_t sleepLastMs_ = 0;
  std::uint32_t sleepTickAccumulatorMs_ = 0;
  std::uint32_t sleepAnimStartMs_ = 0;
  std::uint32_t lastSleepAnimMs_ = 0;
  static constexpr std::uint32_t kSleepTickMs = 600;
  static constexpr std::uint32_t kSleepAnimFrameMs = 150;

  void moveAction(int delta);
  void moveFood(int delta);
  void moveGame(int delta);
  void doAction();
  void eatSelectedFood();
  void launchSelectedGame();
  void wakeUp(const char *toastMsg);

  void drawBar(int x, int y, int w, int h, std::uint8_t value) const;
  void drawActionMenu() const;
  void drawFoodSelect() const;
  void drawFoodIcon(Food food, int cx, int cy) const;
  void drawSleeping() const;
  void drawGameSelect() const;
  void drawGameIcon(Game game, int cx, int cy) const;
};
