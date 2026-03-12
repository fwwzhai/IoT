#pragma once

#include <Arduino.h>

namespace Pins {
constexpr uint8_t kLedRedPin = 6;
constexpr uint8_t kLedYellowPin = 9;
constexpr uint8_t kLedGreenPin = 12;

// Placeholder sensor pins until the final wiring is locked.
constexpr uint8_t kTemperatureSensorPin = 1;
constexpr uint8_t kLightSensorPin = 2;
constexpr uint8_t kMotionSensorPin = 3;
}

