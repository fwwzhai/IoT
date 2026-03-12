#include "Sensors.h"

#include "Config.h"
#include "Pins.h"

namespace {
unsigned long g_lastSampleMs = 0;

int readAnalogClamped(uint8_t pin) {
  const int value = analogRead(pin);
  return value < 0 ? 0 : value;
}
}

void setupSensors() {
  analogReadResolution(12);
  analogSetPinAttenuation(Pins::kTemperatureSensorPin, ADC_11db);
  analogSetPinAttenuation(Pins::kLightSensorPin, ADC_11db);
  pinMode(Pins::kMotionSensorPin, INPUT_PULLUP);
}

void sampleSensors(SensorState& sensorState, unsigned long now) {
  if (now - g_lastSampleMs < Config::kSensorSampleIntervalMs) {
    return;
  }

  g_lastSampleMs = now;

  const int temperatureRaw = readAnalogClamped(Pins::kTemperatureSensorPin);
  const int lightRaw = readAnalogClamped(Pins::kLightSensorPin);

  const float voltage =
    static_cast<float>(temperatureRaw) *
    Config::kAnalogReferenceVolts /
    Config::kAdcMax;

  // TMP36 conversion. Replace if the final build uses a different sensor.
  sensorState.temperatureC = (voltage - 0.5f) * 100.0f;
  sensorState.lightLevel = lightRaw;
  sensorState.motionDetected = digitalRead(Pins::kMotionSensorPin) == LOW;
}

