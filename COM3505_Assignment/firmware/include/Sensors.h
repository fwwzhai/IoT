// Sensors.h
// COM3505 assignment sensor module declarations
//
// This module is responsible for:
//  - configuring sensor GPIO and ADC settings
//  - sampling temperature, light, and optional motion inputs
//  - writing the latest values into SensorState

#ifndef SENSORS_H
#define SENSORS_H

#include "Thing.h"

void setupSensors();
void sampleSensors(SensorState&, unsigned long);
float readTemperatureC();
int readLightLevel();
bool readMotionDetected();
void printSensorSnapshot(const SensorState&);

#endif
