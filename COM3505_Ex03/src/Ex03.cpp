// Ex03.cpp / Ex03.ino
// COM3505 – Exercise 03
//
// This exercise demonstrates:
//  - controlling multiple LEDs (red, yellow, green) to simulate a traffic light
//  - reading a switch to trigger changes
//  - sequencing LED states with delays
//  - using flags to maintain state between button presses

#include "Thing.h"  // include helper functions and previous definitions

// ---------------------------------------------------------------------------
// GPIO pin assignments for LEDs
// Assign pins to red, yellow, and green LEDs. These can be connected on a
// breadboard to visualize a traffic light sequence.
// ---------------------------------------------------------------------------

#ifdef ARDUINO_FEATHER_ESP32
const int red    = 32;
const int yellow = 15;
const int green  = 12;
const int swPin  = 14;
#else
const int red    = 6;
const int yellow = 9;
const int green  = 12;
const int swPin  = 5;
#endif

// ---------------------------------------------------------------------------
// Runtime state
// ON_RED keeps track of whether the traffic light is currently showing red.
// This allows changeLights() to know which state to transition to next.
// ---------------------------------------------------------------------------

bool ON_RED = false;

// Forward declaration
void changeLights();  // changes traffic light LEDs depending on current state
void setup02();
void identifyWiring();

// ---------------------------------------------------------------------------
// setup03()
// Runs once at startup for Exercise 03. Sets up additional LEDs.
// ---------------------------------------------------------------------------

void setup03() {
  setup02();  // call Ex02 setup (serial, built-in LED, switch, etc.)
  
  Serial.println("\nsetup03...");  // print a startup banner for debugging

  // Configure the new LED pins as outputs
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);

  // Interactive wiring check: observe which physical LED lights for each pin.
  identifyWiring();

  // Start the traffic light sequence with green on
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, HIGH);
}


// ---------------------------------------------------------------------------
// loop03()
// Runs repeatedly after setup03() completes.
// Checks the state of the push-button switch to trigger the traffic light.
// ---------------------------------------------------------------------------

void loop03() {
  // Read switch pin (INPUT_PULLUP style, LOW = pressed)
  if (digitalRead(swPin) == LOW) {
    delay(25);  // simple debounce
    if (digitalRead(swPin) != LOW) return;

    Serial.printf(" ON_RED is %d; switch pressed...\n", ON_RED);

    // Change the traffic light LEDs according to the current state
    changeLights();

    // Wait for release to avoid retriggering while held down
    while (digitalRead(swPin) == LOW) {
      delay(10);
    }
  }
}


// ---------------------------------------------------------------------------
// changeLights()
// Updates the traffic light sequence based on the current state (ON_RED).
// Implements simple timing using delay().
// ---------------------------------------------------------------------------

void changeLights() {
  if (ON_RED) {
    // If the light is currently red, transition to green

    // Turn on yellow as part of "red + yellow" combination for 2 seconds
    digitalWrite(yellow, HIGH);
    delay(2000);  // blocking delay to hold the LEDs on

    // Turn off red and yellow, then turn on green
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
  } else {
    // If the light is currently green, transition to red

    // Turn off green and turn yellow on for 3 seconds
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    delay(3000);  // blocking delay

    // Turn off yellow, then turn red on for 5 seconds
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
  }

  // Toggle the state flag so the next call transitions correctly
  ON_RED = !ON_RED;
}

// ---------------------------------------------------------------------------
// identifyWiring()
// Drives each configured LED pin one-by-one so you can verify wiring.
// ---------------------------------------------------------------------------

void identifyWiring() {
  Serial.println("WIRING TEST START");
  Serial.printf("Expect RED LED on GPIO %d\n", red);
  digitalWrite(red, HIGH); delay(1000); digitalWrite(red, LOW); delay(400);

  Serial.printf("Expect YELLOW LED on GPIO %d\n", yellow);
  digitalWrite(yellow, HIGH); delay(1000); digitalWrite(yellow, LOW); delay(400);

  Serial.printf("Expect GREEN LED on GPIO %d\n", green);
  digitalWrite(green, HIGH); delay(1000); digitalWrite(green, LOW); delay(400);

  Serial.printf("Switch pin is GPIO %d (press should read LOW)\n", swPin);
  Serial.println("WIRING TEST END\n");
}
