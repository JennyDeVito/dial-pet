#include "App.h"
#include "../scenes/Dino.h"
#include "../scenes/Menu.h"
#include "../scenes/Pet.h"
#include "../scenes/Pong.h"
#include "GameState.h"
#include "Input.h"
#include "Scene.h"
#include "TimeService.h"
#include <M5Dial.h>

namespace App {
static SceneId currentId_ = SceneId::Menu;
static SceneId requestedId_ = SceneId::Menu;
static bool hasRequest_ = false;

static Menu menuScene;
static Pet petScene;
static Dino dinoScene;
static Pong pongScene;

static Scene *current_ = nullptr;

static Scene *sceneFromId(SceneId id) {
  switch (id) {
  case SceneId::Menu:
    return &menuScene;
  case SceneId::Pet:
    return &petScene;
  case SceneId::Dino:
    return &dinoScene;
  case SceneId::Pong:
    return &pongScene;
  default:
    return &menuScene;
  }
}

void requestScene(SceneId id) {
  requestedId_ = id;
  hasRequest_ = true;
}

void begin() {
  Input::begin();
  GameState::begin(millis());

  TimeService::begin();

  current_ = sceneFromId(SceneId::Menu);
  current_->begin();
}

void loop() {
  TimeService::update();

  GameState::tick(millis());

  const auto &in = Input::state();
  if (hasRequest_) {
    hasRequest_ = false;
    if (requestedId_ != currentId_) {
      current_->end();
      currentId_ = requestedId_;
      current_ = sceneFromId(currentId_);
      current_->begin();
      M5Dial.Display.clear();
    }
  }
  current_->update(in);
  current_->draw();
}
} // namespace App