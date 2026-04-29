// Pins.h
// COM3505 assignment hardware pin map
//
// This file centralises the current wiring assumptions. The LED pins are based
// on the style used in the labs. The sensor pins are still placeholders until
// the final breadboard wiring is confirmed.

#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

namespace Pins {

// ---------------------------------------------------------------------------
// LED pins
// These follow the same red/yellow/green style used in the LED exercises.
// ---------------------------------------------------------------------------

constexpr uint8_t kLedRedPin = 6;
constexpr uint8_t kLedYellowPin = 9;
constexpr uint8_t kLedGreenPin = 12;

// ---------------------------------------------------------------------------
// Sensor pins
// For the current Feather ESP32-S3 target, use the board's analog labels
// directly so the wiring matches what is printed on the header.
// ---------------------------------------------------------------------------

constexpr uint8_t kTemperatureSensorPin = A0;
constexpr uint8_t kButtonSensorPin = 5;

}  // namespace Pins

#endif
