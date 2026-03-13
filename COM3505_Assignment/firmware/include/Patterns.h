// Patterns.h
// COM3505 assignment LED pattern engine declarations
//
// This module owns LED animation logic. It updates the LED buffer
// according to the active pattern and renders the result to the GPIO pins.

#ifndef PATTERNS_H
#define PATTERNS_H

#include "Thing.h"

void setupPatterns();
void updatePattern(DeviceState&, unsigned long);
void renderLeds(const LedBuffer&);
void clearLedBuffer(LedBuffer&);

#endif
