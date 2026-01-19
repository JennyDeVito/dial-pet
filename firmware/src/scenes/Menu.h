#pragma once
#include "App.h"
#include "Scene.h"
#include <cstdint>

class Menu : public Scene {
public:
  void begin() override;
  void update(const Input::InputState &in) override;
  void draw() override;

private:
  static constexpr std::uint8_t kItemCount = 4;
  std::int32_t dialAccumulator_ = 0;
  std::int32_t selected_ = 0;
  bool redraw_ = true;
  void moveSelection(std::int32_t delta);
  void activateSelected();
  int hitTestItem(std::int16_t x, std::int16_t y) const;
};