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
  static constexpr std::uint32_t kPopDurationMs = 140;
  std::int32_t dialAccumulator_ = 0;
  std::int32_t selected_ = 0;
  std::uint32_t popStartMs_ = 0;
  std::int32_t lastSelected_ = 0;
  bool redraw_ = true;
  bool popActive_ = false;
  void moveSelection(std::int32_t delta);
  void activateSelected();
  void drawPreview() const;
  void triggerPop();
  void drawPop() const;
  int hitTestItem(std::int16_t x, std::int16_t y) const;
};