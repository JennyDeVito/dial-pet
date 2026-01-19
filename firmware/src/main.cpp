/*
 * DIAL PET
 * @author JennyDeVito
 * https://github.com/JennyDeVito/dial-pet
 *
 */

#include "App.h"
#include "Input.h"
#include <Arduino.h>
#include <M5Dial.h>

void setup() {
  auto configuration = M5.config();
  M5Dial.begin(configuration, true, false);

  App::begin();
}

void loop() {
  M5Dial.update();
  Input::update();
  App::loop();
}