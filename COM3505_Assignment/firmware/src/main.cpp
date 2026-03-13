// main.cpp
// COM3505 assignment firmware entry point
//
// As in the lab exercises, Arduino only requires setup() and loop() here.
// The real work is delegated to the shared assignment runner in Thing.cpp.

#include <Arduino.h>

#include "Thing.h"

void setup() {
  setupThing();
}

void loop() {
  loopThing();
}
