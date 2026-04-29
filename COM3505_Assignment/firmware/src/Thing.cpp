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
bool g_lastButtonPressed = false;
unsigned long g_lastButtonEdgeMs = 0;

void printStartupBanner() {
  dln(startupDBG, "\nsetupThing...");
  dln(startupDBG, "COM3505 assignment firmware workspace ready.");
  dln(startupDBG, "Structured as Thing + Sensors + Patterns + Network.");
}

PatternId nextPattern(PatternId pattern) {
  switch (pattern) {
    case PatternId::Blink:
      return PatternId::Chase;
    case PatternId::Chase:
      return PatternId::Cycle;
    case PatternId::Cycle:
      return PatternId::Alert;
    case PatternId::Alert:
      return PatternId::Pulse;
    case PatternId::Pulse:
      return PatternId::Heartbeat;
    case PatternId::Heartbeat:
    default:
      return PatternId::Blink;
  }
}

void handleLocalButtonControls(DeviceState& state, unsigned long now) {
  const bool buttonPressed = state.sensors.buttonPressed;

  if (
    buttonPressed &&
    !g_lastButtonPressed &&
    state.mode == DeviceMode::Manual &&
    now - g_lastButtonEdgeMs >= Config::kButtonDebounceMs
  ) {
    state.pattern = nextPattern(state.pattern);
    state.localControlDirty = true;
    g_lastButtonEdgeMs = now;

    dbg(patternDBG, "local button cycle -> ");
    dln(patternDBG, patternName(state.pattern));
  }

  if (buttonPressed != g_lastButtonPressed) {
    g_lastButtonEdgeMs = now;
  }

  g_lastButtonPressed = buttonPressed;
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
  dbg(loopDBG, " button=");
  dbg(loopDBG, state.sensors.buttonPressed);
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
    case PatternId::Pulse:
      return "pulse";
    case PatternId::Heartbeat:
      return "heartbeat";
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
  handleLocalButtonControls(g_deviceState, now);
  updatePattern(g_deviceState, now);
  updateNetwork(g_deviceState, now);

  if (now - g_lastHeartbeatMs >= Config::kHeartbeatIntervalMs) {
    g_lastHeartbeatMs = now;
    printHeartbeat(g_deviceState);
  }
}
