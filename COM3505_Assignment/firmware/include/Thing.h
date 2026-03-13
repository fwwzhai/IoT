// Thing.h
// COM3505 assignment firmware shared declarations
//
// This header plays the same role as the lab "Thing.h" files:
// it exposes the shared state, debug flags, enums, and helper
// function declarations needed across multiple .cpp files.

#ifndef THING_H
#define THING_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Debug flags and lightweight debug macros
// ---------------------------------------------------------------------------

extern bool startupDBG;
extern bool loopDBG;
extern bool sensorDBG;
extern bool patternDBG;
extern bool netDBG;

#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)

// ---------------------------------------------------------------------------
// Device mode and pattern selection
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Sensor state
// These values are updated by the sensor module and consumed by both
// the pattern and network modules.
// ---------------------------------------------------------------------------

struct SensorState {
  float temperatureC = 0.0f;
  int lightLevel = 0;
  bool motionDetected = false;
};

// ---------------------------------------------------------------------------
// LED buffer
// Each field represents the logical state of one LED channel.
// The final assignment may switch this to PWM intensity later.
// ---------------------------------------------------------------------------

struct LedBuffer {
  uint8_t red = 0;
  uint8_t yellow = 0;
  uint8_t green = 0;
};

// ---------------------------------------------------------------------------
// Device state
// Central runtime model shared across firmware modules.
// ---------------------------------------------------------------------------

struct DeviceState {
  DeviceMode mode = DeviceMode::Manual;
  PatternId pattern = PatternId::Blink;
  SensorState sensors;
  LedBuffer leds;
  bool wifiConnected = false;
  bool serverReachable = false;
  unsigned long loopIteration = 0;
};

// ---------------------------------------------------------------------------
// Top-level firmware control
// ---------------------------------------------------------------------------

void setupThing();
void loopThing();

// ---------------------------------------------------------------------------
// Shared accessors and utility functions
// ---------------------------------------------------------------------------

DeviceState& thingState();
const char* modeName(DeviceMode);
const char* patternName(PatternId);

#endif
