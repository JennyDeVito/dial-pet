#include "Input.h"
#include <M5Dial.h>

/* // EXP: encoder reading data type
 * encoder readings are int32_t to restrict them to a 32-bit data type because
 * long may be 64-bit on Linux, in Windows it may be just long 'cause it will
 * be a 32-bit data type for sure - but, to avoid problems, stick to int32_t ;D
 */

namespace Input {
// module variables are static

static InputState state_;
static int32_t encoderLastReading;

void begin() {
  // state_ = {};
  encoderLastReading = M5Dial.Encoder.read();
  state_.dialPosition = encoderLastReading;
}

void update() {
  state_.dialPosition = M5Dial.Encoder.read();
  state_.dialDelta = state_.dialPosition - encoderLastReading;
  encoderLastReading = state_.dialPosition;
  state_.buttonPressed = M5Dial.BtnA.wasPressed();
  state_.buttonDoublePressed = M5Dial.BtnA.wasDoubleClicked();
  auto touch = M5Dial.Touch.getDetail();
  state_.touching = (touch.state & m5::mask_touch) != 0;
  // if (touch.state & m5::mask_touch) {
  //   state_.touching = 1;
  // } else {
  //   state_.touching = 0;
  // }
  state_.x = touch.x;
  state_.y = touch.y;
}

const InputState &state() { return state_; }
} // namespace Input
