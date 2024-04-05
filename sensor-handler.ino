#include "SensorModule.h"

#define BUTTON_PIN 6

const int interval = 5000;
long int lastUpdate = -1;

long int readingsSinceLast = 0;
long int sensorData[3];

bool calibrationMode = false;        // Calibration mode flag
bool lastButtonState = LOW;          // Previous state of the button
unsigned long lastDebounceTime = -1;  // Last time the button was pressed
unsigned long debounceDelay = 200;   // Debounce delay time in milliseconds

void setup() {
  Serial.begin(9600);        // Initialize serial communication
  initializeSensorModule();  // Initialize sensor module
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  long int currentTime = millis();  // Get current time

  readSensorData(sensorData, calibrationMode);  // Read sensor data
  readingsSinceLast += 1;

  bool currentButtonState = digitalRead(BUTTON_PIN);

  // Check if the calibration button is pressed and the debounce delay has
  // elapsed

  if (currentButtonState == HIGH) {
    Serial.println("HIGH");
  }
  if (currentButtonState == HIGH && currentButtonState != lastButtonState &&
      (millis() - lastDebounceTime) > debounceDelay) {
    Serial.println("Button pressed");
    calibrationMode = !calibrationMode;  // Toggle calibration mode
    lastDebounceTime = millis();         // Update the last debounce time
  }

  lastButtonState = currentButtonState;  // Update the last button state

  if (calibrationMode) {
    calibrateSensors();
  }

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
    Serial.println(calibrationMode);
    readingsSinceLast = 0;
  }
}
