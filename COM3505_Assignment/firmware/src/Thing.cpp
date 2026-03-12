#include "Thing.h"

#include "Config.h"
#include "Network.h"
#include "Patterns.h"
#include "Sensors.h"

namespace {
DeviceState g_deviceState;
unsigned long g_lastHeartbeatMs = 0;
}

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

  Serial.println("\nCOM3505 assignment firmware workspace ready.");
  Serial.println("Structured as Thing + Sensors + Patterns + Network.");

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
    Serial.printf(
      "mode=%s pattern=%s wifi=%d server=%d temp=%.1f light=%d motion=%d loops=%lu\n",
      modeName(g_deviceState.mode),
      patternName(g_deviceState.pattern),
      g_deviceState.wifiConnected,
      g_deviceState.serverReachable,
      g_deviceState.sensors.temperatureC,
      g_deviceState.sensors.lightLevel,
      g_deviceState.sensors.motionDetected,
      g_deviceState.loopIteration
    );
  }
}

