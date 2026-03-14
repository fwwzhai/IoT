// Patterns.cpp
// COM3505 assignment LED pattern engine
//
// This module takes the role of the LED exercises and generalises them:
// pattern logic is separated from the main loop and rendered from a buffer.

#include "Patterns.h"

#include "Config.h"
#include "Pins.h"

namespace {
unsigned long g_lastAnimationMs = 0;
uint8_t g_animationStep = 0;
PatternId g_lastPattern = PatternId::Blink;
DeviceMode g_lastMode = DeviceMode::Manual;

// ---------------------------------------------------------------------------
// animationIntervalFor()
// Different patterns can advance at different visual speeds.
// ---------------------------------------------------------------------------

unsigned long animationIntervalFor(PatternId pattern) {
  switch (pattern) {
    case PatternId::Alert:
      return 120;
    case PatternId::Chase:
      return 180;
    case PatternId::Cycle:
      return 350;
    case PatternId::Blink:
    default:
      return Config::kAnimationIntervalMs;
  }
}

// ---------------------------------------------------------------------------
// writeLedPin()
// Convert a buffer value into a simple digital on/off output.
// ---------------------------------------------------------------------------

void writeLedPin(uint8_t pin, uint8_t value) {
  digitalWrite(pin, value > 0 ? HIGH : LOW);
}

// ---------------------------------------------------------------------------
// setBuffer()
// Convenience helper for assigning all three LED channels in one call.
// ---------------------------------------------------------------------------

void setBuffer(DeviceState& state, uint8_t red, uint8_t yellow, uint8_t green) {
  state.leds.red = red;
  state.leds.yellow = yellow;
  state.leds.green = green;
}

// ---------------------------------------------------------------------------
// applyAutoPattern()
// Select a pattern automatically from sensor thresholds.
// ---------------------------------------------------------------------------

void applyAutoPattern(DeviceState& state) {
  if (Config::kTmp36OnlyFirstTestMode) {
    if (state.sensors.temperatureC >= Config::kTemperatureAlertC) {
      state.pattern = PatternId::Alert;
      return;
    }

    if (state.sensors.temperatureC >= Config::kTemperatureWarningC) {
      state.pattern = PatternId::Chase;
      return;
    }

    state.pattern = PatternId::Blink;
    return;
  }

  if (
    state.sensors.temperatureC >= Config::kTemperatureAlertC ||
    state.sensors.lightLevel <= Config::kLightAlertLevel ||
    state.sensors.motionDetected
  ) {
    state.pattern = PatternId::Alert;
    return;
  }

  if (
    state.sensors.temperatureC >= Config::kTemperatureWarningC ||
    state.sensors.lightLevel <= Config::kLightWarningLevel
  ) {
    state.pattern = PatternId::Chase;
    return;
  }

  state.pattern = PatternId::Cycle;
}

// ---------------------------------------------------------------------------
// Pattern stepping helpers
// Each one writes a new buffer state based on g_animationStep.
// ---------------------------------------------------------------------------

void stepBlinkPattern(DeviceState& state) {
  if (g_animationStep % 2 == 0) {
    setBuffer(state, 255, 255, 255);
  } else {
    clearLedBuffer(state.leds);
  }
}

void stepChasePattern(DeviceState& state) {
  switch (g_animationStep % 3) {
    case 0:
      setBuffer(state, 255, 0, 0);
      break;
    case 1:
      setBuffer(state, 0, 255, 0);
      break;
    default:
      setBuffer(state, 0, 0, 255);
      break;
  }
}

void stepCyclePattern(DeviceState& state) {
  switch (g_animationStep % 4) {
    case 0:
      setBuffer(state, 255, 0, 0);
      break;
    case 1:
      setBuffer(state, 0, 255, 0);
      break;
    case 2:
      setBuffer(state, 0, 0, 255);
      break;
    default:
      clearLedBuffer(state.leds);
      break;
  }
}

void stepAlertPattern(DeviceState& state) {
  if (g_animationStep % 2 == 0) {
    setBuffer(state, 255, 0, 0);
  } else {
    setBuffer(state, 255, 255, 0);
  }
}

// ---------------------------------------------------------------------------
// stepPattern()
// Dispatch to the current animation implementation.
// ---------------------------------------------------------------------------

void stepPattern(DeviceState& state) {
  switch (state.pattern) {
    case PatternId::Blink:
      stepBlinkPattern(state);
      break;

    case PatternId::Chase:
      stepChasePattern(state);
      break;

    case PatternId::Cycle:
      stepCyclePattern(state);
      break;

    case PatternId::Alert:
      stepAlertPattern(state);
      break;
  }

  renderLeds(state.leds);
  g_animationStep++;
}
}

// ---------------------------------------------------------------------------
// clearLedBuffer()
// Set all channels low.
// ---------------------------------------------------------------------------

void clearLedBuffer(LedBuffer& leds) {
  leds.red = 0;
  leds.yellow = 0;
  leds.green = 0;
}

// ---------------------------------------------------------------------------
// renderLeds()
// Push the logical LED buffer to the GPIO pins.
// ---------------------------------------------------------------------------

void renderLeds(const LedBuffer& leds) {
  writeLedPin(Pins::kLedRedPin, leds.red);
  writeLedPin(Pins::kLedYellowPin, leds.yellow);
  writeLedPin(Pins::kLedGreenPin, leds.green);
}

// ---------------------------------------------------------------------------
// setupPatterns()
// Configure GPIO and clear the LEDs.
// ---------------------------------------------------------------------------

void setupPatterns() {
  pinMode(Pins::kLedRedPin, OUTPUT);
  pinMode(Pins::kLedYellowPin, OUTPUT);
  pinMode(Pins::kLedGreenPin, OUTPUT);

  LedBuffer leds;
  clearLedBuffer(leds);
  renderLeds(leds);

  dln(startupDBG, "setupPatterns...");
}

// ---------------------------------------------------------------------------
// updatePattern()
// Auto-select pattern if needed and step the active animation on schedule.
// ---------------------------------------------------------------------------

void updatePattern(DeviceState& state, unsigned long now) {
  if (state.mode == DeviceMode::Auto) {
    applyAutoPattern(state);
  }

  if (state.pattern != g_lastPattern || state.mode != g_lastMode) {
    g_animationStep = 0;
    g_lastPattern = state.pattern;
    g_lastMode = state.mode;
  }

  if (now - g_lastAnimationMs < animationIntervalFor(state.pattern)) {
    return;
  }

  g_lastAnimationMs = now;
  dbg(patternDBG, "stepping pattern ");
  dln(patternDBG, patternName(state.pattern));
  stepPattern(state);
}
