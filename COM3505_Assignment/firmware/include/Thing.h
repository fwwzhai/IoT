#pragma once

#include <Arduino.h>

enum class DeviceMode {
  Manual,
  Auto,
};

enum class PatternId {
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

struct DeviceState {
  DeviceMode mode = DeviceMode::Manual;
  PatternId pattern = PatternId::Blink;
  SensorState sensors;
  bool wifiConnected = false;
};

void setupThing();
void loopThing();

