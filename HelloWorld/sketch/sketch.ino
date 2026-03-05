// Ex02 + Ex03: traffic lights controlled by a push-button.
#include <Arduino.h>

// ESP32-S3 Feather mapping from old worksheet labels:
// old 32 -> 6, old 15 -> 9, old 12 -> 12
const int ledRed = 6;
const int ledYellow = 9;
const int ledGreen = 12;
const int pushButton = 5;

// Traffic-light phase durations (ms)
const unsigned long redMs = 1500;
const unsigned long redYellowMs = 800;
const unsigned long greenMs = 1500;
const unsigned long yellowMs = 800;

enum LightState {
  STATE_IDLE,
  STATE_RED,
  STATE_RED_YELLOW,
  STATE_GREEN,
  STATE_YELLOW
};

LightState state = STATE_IDLE;
unsigned long stateStartMs = 0;
bool running = false;
bool lastButtonPressed = false;

void setLights(bool redOn, bool yellowOn, bool greenOn) {
  digitalWrite(ledRed, redOn ? HIGH : LOW);
  digitalWrite(ledYellow, yellowOn ? HIGH : LOW);
  digitalWrite(ledGreen, greenOn ? HIGH : LOW);
}

void goToState(LightState next) {
  state = next;
  stateStartMs = millis();
  switch (state) {
    case STATE_IDLE:       setLights(false, false, false); break;
    case STATE_RED:        setLights(true,  false, false); break;
    case STATE_RED_YELLOW: setLights(true,  true,  false); break;
    case STATE_GREEN:      setLights(false, false, true ); break;
    case STATE_YELLOW:     setLights(false, true,  false); break;
  }
}

void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP); // pressed -> LOW
  goToState(STATE_IDLE);
}

void loop() {
  unsigned long now = millis();
  bool buttonPressed = (digitalRead(pushButton) == LOW);

  // Trigger on button press edge.
  if (buttonPressed && !lastButtonPressed) {
    if (!running) {
      running = true;
      goToState(STATE_RED);
    } else {
      running = false;
      goToState(STATE_IDLE);
    }
  }
  lastButtonPressed = buttonPressed;

  if (!running) return;

  unsigned long elapsed = now - stateStartMs;
  switch (state) {
    case STATE_RED:
      if (elapsed >= redMs) goToState(STATE_RED_YELLOW);
      break;
    case STATE_RED_YELLOW:
      if (elapsed >= redYellowMs) goToState(STATE_GREEN);
      break;
    case STATE_GREEN:
      if (elapsed >= greenMs) goToState(STATE_YELLOW);
      break;
    case STATE_YELLOW:
      if (elapsed >= yellowMs) goToState(STATE_RED);
      break;
    case STATE_IDLE:
      break;
  }
}
