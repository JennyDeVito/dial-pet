#include "TimeService.h"
#include "Secrets.h"
#include <M5Dial.h>
#include <WiFi.h>
#include <time.h>

namespace TimeService {
enum class Stage : std::uint8_t {
  Idle,
  WifiConnecting,
  NtpStarting,
  WaitingTime,
  Synced,
  Failed
};

static Stage stage_ = Stage::Idle;
static std::uint32_t lastAttemptMs_ = 0;
static std::uint32_t epochUtc_ = 0;

static constexpr std::uint32_t kWifiRetryMs = 5000;
static constexpr std::uint32_t kTimeWaitMs = 15000;

static constexpr const char *NTP_TIMEZONE = "UTC+3"; // POSIX timezone
static constexpr const char *NTP_SERVER1 = "pool.ntp.org";
static constexpr const char *NTP_SERVER2 = "time.nist.gov";
static constexpr const char *NTP_SERVER3 = "time.google.com";

bool wifiConnected() { return WiFi.status() == WL_CONNECTED; }

static bool timeIsValid() {
  time_t now = time(nullptr);
  return now > 1577836800;
}

void begin() {
  stage_ = Stage::WifiConnecting;
  lastAttemptMs_ = 0;

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void update() {
  const std::uint32_t nowMs = millis();
  switch (stage_) {
  case Stage::Idle:
    return;

  case Stage::WifiConnecting: {
    if (wifiConnected()) {
      stage_ = Stage::NtpStarting;
    } else if (nowMs - lastAttemptMs_ > kWifiRetryMs) {
      lastAttemptMs_ = nowMs;
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
  } break;

  case Stage::NtpStarting: {
    configTzTime(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
    lastAttemptMs_ = nowMs;
    stage_ = Stage::WaitingTime;
  } break;

  case Stage::WaitingTime: {
    if (timeIsValid()) {
      time_t t = time(nullptr) + 1;
      while (t > time(nullptr)) {
      }

      if (M5Dial.Rtc.isEnabled()) {
        M5Dial.Rtc.setDateTime(gmtime(&t));
      }

      epochUtc_ = static_cast<std::uint32_t>(t);
      stage_ = Stage::Synced;
    } else if (nowMs - lastAttemptMs_ > kTimeWaitMs) {
      stage_ = Stage::Failed;
    }
  } break;

  case Stage::Synced:
    if (timeIsValid()) {
      epochUtc_ = static_cast<std::uint32_t>(time(nullptr));
    }
    return;

  case Stage::Failed:
    return;
  }
}

bool synced() { return stage_ == Stage::Synced; }

std::uint32_t epochUtc() { return epochUtc_; }
} // namespace TimeService