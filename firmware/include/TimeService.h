#pragma once
#include <cstdint>

namespace TimeService {
void begin();
void update();
bool synced();
std::uint32_t epochUtc();
bool wifiConnected();
} // namespace TimeService
