#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <Arduino.h>

void initializeSensorModule();
void readSensorData(int dataOut[]);
void sendData(int dataOut[]);
void updateDisplay(int dataOut[]);

#endif
