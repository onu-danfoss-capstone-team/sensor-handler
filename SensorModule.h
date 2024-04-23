#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <Arduino.h>

void initializeSensorModule();
void readSensorData(long int dataOut[], bool testEnabled);
void sendData(long int dataOut[], long int readingsSinceLast, bool testEnabled);
void updateDisplay(long int dataOut[], long int readingsSinceLast, bool testEnabled);
void calibrateSensors();

#endif
