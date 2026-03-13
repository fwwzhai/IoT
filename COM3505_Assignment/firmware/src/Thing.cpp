// Thing.cpp
// COM3505 assignment top-level firmware runner
//
// This file is the equivalent of the lab "exercise runner":
// it owns shared globals, startup sequencing, and the high-level loop.

#include "Thing.h"

#include "Config.h"
#include "Network.h"
#include "Patterns.h"
#include "Sensors.h"

namespace {
DeviceState g_deviceState;
unsigned long g_lastHeartbeatMs = 0;

void printStartupBanner() {
  dln(startupDBG, "\nsetupThing...");
  dln(startupDBG, "COM3505 assignment firmware workspace ready.");
  dln(startupDBG, "Structured as Thing + Sensors + Patterns + Network.");
}

void printHeartbeat(const DeviceState& state) {
  dbg(loopDBG, "mode=");
  dbg(loopDBG, modeName(state.mode));
  dbg(loopDBG, " pattern=");
  dbg(loopDBG, patternName(state.pattern));
  dbg(loopDBG, " wifi=");
  dbg(loopDBG, state.wifiConnected);
  dbg(loopDBG, " server=");
  dbg(loopDBG, state.serverReachable);
  dbg(loopDBG, " temp=");
  dbg(loopDBG, state.sensors.temperatureC);
  dbg(loopDBG, " light=");
  dbg(loopDBG, state.sensors.lightLevel);
  dbg(loopDBG, " motion=");
  dbg(loopDBG, state.sensors.motionDetected);
  dbg(loopDBG, " loops=");
  dln(loopDBG, state.loopIteration);
}
}

// ---------------------------------------------------------------------------
// Global debug flag definitions
// ---------------------------------------------------------------------------

bool startupDBG = true;
bool loopDBG = true;
bool sensorDBG = true;
bool patternDBG = true;
bool netDBG = true;

DeviceState& thingState() {
  return g_deviceState;
}

const char* modeName(DeviceMode mode) {
  switch (mode) {
    case DeviceMode::Manual:
      return "manual";
    case DeviceMode::Auto:
      return "auto";
    default:
      return "unknown";
  }
}

const char* patternName(PatternId pattern) {
  switch (pattern) {
    case PatternId::Blink:
      return "blink";
    case PatternId::Chase:
      return "chase";
    case PatternId::Cycle:
      return "cycle";
    case PatternId::Alert:
      return "alert";
    default:
      return "unknown";
  }
}

void setupThing() {
  Serial.begin(115200);
  delay(250);

  printStartupBanner();

  setupSensors();
  setupPatterns();
  setupNetwork(g_deviceState);
}

void loopThing() {
  const unsigned long now = millis();

  g_deviceState.loopIteration++;

  sampleSensors(g_deviceState.sensors, now);
  updatePattern(g_deviceState, now);
  updateNetwork(g_deviceState, now);

  if (now - g_lastHeartbeatMs >= Config::kHeartbeatIntervalMs) {
    g_lastHeartbeatMs = now;
    printHeartbeat(g_deviceState);
  }
}
