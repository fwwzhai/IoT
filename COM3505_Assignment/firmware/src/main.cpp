#include <Arduino.h>

#include "Thing.h"

namespace {
DeviceState deviceState;
unsigned long lastHeartbeatMs = 0;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = 1000;
}

void setup() {
  setupThing();
}

void loop() {
  loopThing();
}

void setupThing() {
  Serial.begin(115200);
  delay(250);
  Serial.println("\nCOM3505 assignment firmware workspace ready.");
  Serial.println("Next step: pin map, sensor drivers, Wi-Fi, and pattern engine.");
}

void loopThing() {
  const unsigned long now = millis();

  // Temporary heartbeat until the real scheduler is in place.
  if (now - lastHeartbeatMs >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeatMs = now;
    Serial.printf(
      "mode=%d pattern=%d wifi=%d temp=%.1f light=%d motion=%d\n",
      static_cast<int>(deviceState.mode),
      static_cast<int>(deviceState.pattern),
      deviceState.wifiConnected,
      deviceState.sensors.temperatureC,
      deviceState.sensors.lightLevel,
      deviceState.sensors.motionDetected
    );
  }
}

