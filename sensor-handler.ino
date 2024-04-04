#include "SensorModule.h"

#define buttonPin 5

bool testEnabled = false;
const int interval = 5000;

const int lastDebounce = -1;
long int lastUpdate = -1;

int lastButtonState = 0;
int newButtonState = 0;
long int readingsSinceLast = 0;
long int sensorData[3];

void setup() {
  Serial.begin(9600);        // Initialize serial communication
  initializeSensorModule();  // Initialize sensor module
}

void loop() {
  long int currentTime = millis();  // Get current time

  readSensorData(sensorData, testEnabled);  // Read sensor data
  readingsSinceLast += 1;

  if (currentTime - lastUpdate >= interval) {
    lastUpdate = currentTime;  // Update last serial update time

    updateDisplay(sensorData,
                  readingsSinceLast);  // Update display (every 1 second)
    sendData(
        sensorData,
        readingsSinceLast);  // Send sensor data over serial (every 15 minutes)

    sensorData[0] = 0;
    sensorData[1] = 0;
    sensorData[2] = 0;
    Serial.println(testEnabled);
    readingsSinceLast = 0;
  }

  newButtonState = digitalRead(buttonPin);

  if (lastButtonState != newButtonState && newButtonState == 0) {
    testEnabled = !testEnabled;
  }

  lastButtonState = newButtonState;
}
