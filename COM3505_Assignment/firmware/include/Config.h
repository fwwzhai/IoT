#pragma once

#include <Arduino.h>

namespace Config {
constexpr unsigned long kHeartbeatIntervalMs = 1000;
constexpr unsigned long kSensorSampleIntervalMs = 2000;
constexpr unsigned long kNetworkReconnectIntervalMs = 10000;
constexpr unsigned long kNetworkSyncIntervalMs = 2000;
constexpr unsigned long kAnimationIntervalMs = 250;

constexpr float kAnalogReferenceVolts = 3.3f;
constexpr float kAdcMax = 4095.0f;

constexpr float kTemperatureWarningC = 28.0f;
constexpr float kTemperatureAlertC = 32.0f;
constexpr int kLightWarningLevel = 1800;
constexpr int kLightAlertLevel = 900;
}

