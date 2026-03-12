#pragma once

#include <Arduino.h>

enum class DeviceMode : uint8_t {
  Manual,
  Auto,
};

enum class PatternId : uint8_t {
  Blink,
  Chase,
  Cycle,
  Alert,
};

struct SensorState {
  float temperatureC = 0.0f;
  int lightLevel = 0;
  bool motionDetected = false;
};

struct LedBuffer {
  uint8_t red = 0;
  uint8_t yellow = 0;
  uint8_t green = 0;
};

struct DeviceState {
  DeviceMode mode = DeviceMode::Manual;
  PatternId pattern = PatternId::Blink;
  SensorState sensors;
  LedBuffer leds;
  bool wifiConnected = false;
  bool serverReachable = false;
  unsigned long loopIteration = 0;
};

void setupThing();
void loopThing();
DeviceState& thingState();
const char* modeName(DeviceMode mode);
const char* patternName(PatternId pattern);
