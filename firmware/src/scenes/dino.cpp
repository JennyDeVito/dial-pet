#include "Dino.h"
#include "App.h"
#include <M5Dial.h>

void Dino::begin() { redraw_ = true; }

void Dino::update(const Input::InputState &in) {
  if (in.buttonPressed) {
    App::requestScene(App::SceneId::Menu);
  }
}

void Dino::draw() {
  if (!redraw_) {
    return;
  }
  redraw_ = false;

  M5Dial.Display.clear();
  M5Dial.Display.setTextDatum(middle_center);
  M5Dial.Display.setTextSize(2);
  M5Dial.Display.drawString("Dino SCENE", 120, 120);
  M5Dial.Display.setTextSize(1);
  M5Dial.Display.drawString("Press button to go back", 120, 160);
}