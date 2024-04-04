#include "SensorModule.h"

const int interval = 5000;
long int lastUpdate = -1;

void setup() {
    Serial.begin(9600); // Initialize serial communication
    initializeSensorModule(); // Initialize sensor module
}

void loop() {
    int sensorData[3]; // Array to store sensor data
    long int currentTime = millis();    // Get current time

    if (currentTime - lastUpdate >= interval) {
        lastUpdate = currentTime; // Update last serial update time
        readSensorData(sensorData); // Read sensor data
        sendData(sensorData); // Send sensor data over serial (every 15 minutes)
    }

    updateDisplay(sensorData); // Update display (every 1 second)
}
