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
// Bring-up mode
// Enable this while testing with TMP36 + 3 LEDs only.
// ---------------------------------------------------------------------------

constexpr bool kTmp36OnlyFirstTestMode = true;

// ---------------------------------------------------------------------------
// Timing configuration
// ---------------------------------------------------------------------------

constexpr unsigned long kHeartbeatIntervalMs = 1000;
constexpr unsigned long kSensorSampleIntervalMs = 2000;
constexpr unsigned long kNetworkReconnectIntervalMs = 10000;
constexpr unsigned long kSensorUploadIntervalMs = 2000;
constexpr unsigned long kControlPollIntervalMs = 500;
constexpr unsigned long kAnimationIntervalMs = 250;
constexpr unsigned long kSocketTimeoutMs = 3000;
constexpr unsigned long kButtonDebounceMs = 180;

// ---------------------------------------------------------------------------
// ADC conversion constants
// ---------------------------------------------------------------------------

constexpr float kAnalogReferenceVolts = 3.3f;
constexpr float kAdcMax = 4095.0f;

// ---------------------------------------------------------------------------
// Auto-mode thresholds
// These are starting values only and should be tuned after hardware tests.
// ---------------------------------------------------------------------------

constexpr float kTemperatureWarningC = 21.0f;
constexpr float kTemperatureAlertC = 17.0f;
constexpr int kLightWarningLevel = 1800;
constexpr int kLightAlertLevel = 900;

}  // namespace Config

#endif
