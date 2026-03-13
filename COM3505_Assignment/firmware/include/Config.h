// Config.h
// COM3505 assignment configuration constants
//
// This file groups timing and threshold constants in one place so the
// assignment can be tuned without hunting through multiple source files.

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {

// ---------------------------------------------------------------------------
// Timing configuration
// ---------------------------------------------------------------------------

constexpr unsigned long kHeartbeatIntervalMs = 1000;
constexpr unsigned long kSensorSampleIntervalMs = 2000;
constexpr unsigned long kNetworkReconnectIntervalMs = 10000;
constexpr unsigned long kNetworkSyncIntervalMs = 2000;
constexpr unsigned long kAnimationIntervalMs = 250;

// ---------------------------------------------------------------------------
// ADC conversion constants
// ---------------------------------------------------------------------------

constexpr float kAnalogReferenceVolts = 3.3f;
constexpr float kAdcMax = 4095.0f;

// ---------------------------------------------------------------------------
// Auto-mode thresholds
// These are starting values only and should be tuned after hardware tests.
// ---------------------------------------------------------------------------

constexpr float kTemperatureWarningC = 28.0f;
constexpr float kTemperatureAlertC = 32.0f;
constexpr int kLightWarningLevel = 1800;
constexpr int kLightAlertLevel = 900;

}  // namespace Config

#endif
