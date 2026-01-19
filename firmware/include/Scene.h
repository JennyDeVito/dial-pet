#pragma once
#include "Input.h"

class Scene {
public:
  virtual ~Scene() = default;
  virtual void begin() {}
  virtual void end() {}
  virtual void update(const Input::InputState &in) = 0;
  virtual void draw() = 0;
};
