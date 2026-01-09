#include <M5Dial.h>

void setup() {
  auto cfg = M5.config();
  M5Dial.begin(cfg, true, false);

  M5Dial.Display.setTextColor(GREEN);
  M5Dial.Display.setRotation(0);
  M5Dial.Display.setTextFont(&fonts::FreeMono9pt7b);
  M5Dial.Display.setTextDatum(middle_center);
  M5Dial.Display.setTextSize(2);
  M5Dial.Display.fillScreen(TFT_BLACK);

  M5Dial.Display.drawString("DIAL INPUT TEST", M5Dial.Display.width() / 2,
                            M5Dial.Display.height() / 2);
}

// lastReading is int32_t to restrict it to 32-bit long may be 64-bit on Linux
int32_t lastReading = -999;

void loop() {
  M5Dial.update();

  int32_t encoderReading = M5Dial.Encoder.read();
  int32_t delta = encoderReading - lastReading;

  if (encoderReading != lastReading) {
    M5Dial.Speaker.tone(1200, 15);
    M5Dial.Display.clear();
    lastReading = encoderReading;
    Serial.println(encoderReading);
    M5Dial.Display.drawString(String(encoderReading),
                              M5Dial.Display.width() / 2,
                              M5Dial.Display.height() / 2);
  }

  if (M5Dial.BtnA.wasPressed()) {
    M5Dial.Encoder.readAndReset();
  }
}