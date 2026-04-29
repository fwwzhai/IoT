// Sensors.cpp
// COM3505 assignment sensor module
//
// This module follows the sensor style used in the later labs:
// configure ADC once, then sample the inputs on a timed schedule.

#include "Sensors.h"

#include "Config.h"
#include "Pins.h"

namespace {
unsigned long g_lastSampleMs = 0;
int g_lastTemperatureRaw = 0;
float g_lastTemperatureVoltage = 0.0f;

// ---------------------------------------------------------------------------
// ADC helper
// Reads and clamps the value into a safe non-negative range.
// ---------------------------------------------------------------------------

int readAnalogClamped(uint8_t pin) {
  const int value = analogRead(pin);
  return value < 0 ? 0 : value;
}
}

// ---------------------------------------------------------------------------
// setupSensors()
// Configure analog and digital sensor inputs.
// ---------------------------------------------------------------------------

void setupSensors() {
  analogReadResolution(12);
  analogSetPinAttenuation(Pins::kTemperatureSensorPin, ADC_11db);
  pinMode(Pins::kButtonSensorPin, INPUT_PULLUP);

  dln(startupDBG, "setupSensors...");
  dbg(startupDBG, "temperature pin = ");
  dln(startupDBG, Pins::kTemperatureSensorPin);
  dbg(startupDBG, "button pin = ");
  dln(startupDBG, Pins::kButtonSensorPin);
}

// ---------------------------------------------------------------------------
// readTemperatureC()
// Convert the current temperature sensor voltage into degrees Celsius.
// TMP36 conversion is used as the starting default.
// ---------------------------------------------------------------------------

float readTemperatureC() {
  const int temperatureRaw = readAnalogClamped(Pins::kTemperatureSensorPin);
  const float voltage =
    static_cast<float>(temperatureRaw) *
    Config::kAnalogReferenceVolts /
    Config::kAdcMax;

  g_lastTemperatureRaw = temperatureRaw;
  g_lastTemperatureVoltage = voltage;

  return (voltage - 0.5f) * 100.0f;
}

// readButtonPressed()
// Read the digital push-button input using the internal pull-up.
// ---------------------------------------------------------------------------

bool readButtonPressed() {
  return digitalRead(Pins::kButtonSensorPin) == LOW;
}

// ---------------------------------------------------------------------------
// printSensorSnapshot()
// Lightweight debug output for current sensor values.
// ---------------------------------------------------------------------------

void printSensorSnapshot(const SensorState& sensorState) {
  dbg(sensorDBG, "sensor snapshot: raw=");
  dbg(sensorDBG, g_lastTemperatureRaw);
  dbg(sensorDBG, " volts=");
  dbg(sensorDBG, g_lastTemperatureVoltage);
  dbg(sensorDBG, " temp=");
  dbg(sensorDBG, sensorState.temperatureC);
  dbg(sensorDBG, " button=");
  dln(sensorDBG, sensorState.buttonPressed);
}

// ---------------------------------------------------------------------------
// sampleSensors()
// Sample all sensors on the configured interval and update SensorState.
// ---------------------------------------------------------------------------

void sampleSensors(SensorState& sensorState, unsigned long now) {
  sensorState.buttonPressed = readButtonPressed();

  if (now - g_lastSampleMs < Config::kSensorSampleIntervalMs) {
    return;
  }

  g_lastSampleMs = now;

  sensorState.temperatureC = readTemperatureC();

  printSensorSnapshot(sensorState);
}
