#include "SensorModule.h"

void setup() {
    Serial.begin(9600); // Initialize serial communication
    initializeSensorModule(); // Initialize sensor module
}

void loop() {
    int sensorData[3]; // Array to store sensor data

    readSensorData(sensorData); // Read sensor data
    sendData(sensorData); // Send sensor data over serial (every 15 minutes)
    updateDisplay(sensorData); // Update display (every 1 second)
}
