#include "Patterns.h"

#include "Config.h"
#include "Pins.h"

namespace {
unsigned long g_lastAnimationMs = 0;
uint8_t g_animationStep = 0;
PatternId g_lastPattern = PatternId::Blink;
DeviceMode g_lastMode = DeviceMode::Manual;

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

void writeLedPin(uint8_t pin, uint8_t value) {
  digitalWrite(pin, value > 0 ? HIGH : LOW);
}

void renderBuffer(const LedBuffer& leds) {
  writeLedPin(Pins::kLedRedPin, leds.red);
  writeLedPin(Pins::kLedYellowPin, leds.yellow);
  writeLedPin(Pins::kLedGreenPin, leds.green);
}

void setBuffer(DeviceState& state, uint8_t red, uint8_t yellow, uint8_t green) {
  state.leds.red = red;
  state.leds.yellow = yellow;
  state.leds.green = green;
}

void applyAutoPattern(DeviceState& state) {
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

void stepPattern(DeviceState& state) {
  switch (state.pattern) {
    case PatternId::Blink:
      if (g_animationStep % 2 == 0) {
        setBuffer(state, 255, 255, 255);
      } else {
        setBuffer(state, 0, 0, 0);
      }
      break;

    case PatternId::Chase:
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
      break;

    case PatternId::Cycle:
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
          setBuffer(state, 0, 0, 0);
          break;
      }
      break;

    case PatternId::Alert:
      if (g_animationStep % 2 == 0) {
        setBuffer(state, 255, 0, 0);
      } else {
        setBuffer(state, 255, 255, 0);
      }
      break;
  }

  renderBuffer(state.leds);
  g_animationStep++;
}
}

void setupPatterns() {
  pinMode(Pins::kLedRedPin, OUTPUT);
  pinMode(Pins::kLedYellowPin, OUTPUT);
  pinMode(Pins::kLedGreenPin, OUTPUT);

  renderBuffer(LedBuffer{});
}

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
  stepPattern(state);
}

