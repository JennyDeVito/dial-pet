#pragma once
#include "Scene.h"

class Dino : public Scene {
public:
  void begin() override;
  void update(const Input::InputState &in) override;
  void draw() override;

private:
  bool redraw_ = true;
};