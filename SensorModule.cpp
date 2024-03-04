#include "SensorModule.h"   // Include sensor module header file
#include "LedControl.h"     // Include LED control library

LedControl lc = LedControl(11, 13, 12, 8);  // Initialize LED control pins
#define ovenTempPin A0       // Define pin for oven temperature sensor
#define oilTempPin A1        // Define pin for oil temperature sensor
#define oilPressurePin A2    // Define pin for oil pressure sensor
#define CS 7                 // Define chip select pin for SPI communication

const uint32_t displayInterval = 100;   // Display update interval in milliseconds
const uint32_t serialInterval = 900000; // Serial update interval in milliseconds

static uint32_t lastDisplayUpdate = 0;   // Variable to store last display update time
static uint32_t lastSerialUpdate = 0;    // Variable to store last serial update time

void initializeSensorModule() {
    pinMode(ovenTempPin, INPUT);        // Set oven temperature pin as input
    pinMode(oilTempPin, INPUT);         // Set oil temperature pin as input
    pinMode(oilPressurePin, INPUT);     // Set oil pressure pin as input
    pinMode(CS, OUTPUT);                 // Set chip select pin as output
}

void readSensorData(int dataOut[]) {
    // Read sensor data and map to meaningful values
    dataOut[0] = map(analogRead(ovenTempPin), 0, 1023, -50, 450);        // Read oven temperature
    dataOut[1] = map(analogRead(oilTempPin), 0, 1023, -50, 450);         // Read oil temperature
    dataOut[2] = map(analogRead(oilPressurePin), 204, 1023, 0, 500);     // Read oil pressure
}

void writeFourDigitNumber(int offset, int n) {
    int digit = n % 10;  // Get the least significant digit
    int truncatedOffset = offset % 8;   // Truncate offset to fit within 8 digits
    for (int i = 0; i < 4; i++) {
        lc.setDigit(floor(offset / 8), truncatedOffset + i, digit, false);  // Write digit to LED display
        n = n / 10;         // Move to the next digit
        digit = n % 10;     // Get the next digit
    }
}

void sendData(int dataOut[]) {
    uint32_t currentTime = millis();    // Get current time

    // Send data over serial at regular intervals
    if (currentTime - lastSerialUpdate >= serialInterval) {
        lastSerialUpdate = currentTime; // Update last serial update time

        // Print sensor data over serial
        Serial.print("Oven Temp: ");
        Serial.println(dataOut[0]);
        Serial.print("Oil Temp: ");
        Serial.println(dataOut[1]);
        Serial.print("Oil Pressure: ");
        Serial.println(dataOut[2]);
    }
}

void updateDisplay(int dataOut[]) {
    uint32_t currentTime = millis();    // Get current time

    // Update LED display at regular intervals
    if (currentTime - lastDisplayUpdate >= displayInterval) {
        lastDisplayUpdate = currentTime; // Update last display update time

        // Update LED display with sensor data
        writeFourDigitNumber(0, dataOut[0]);   // Display oven temperature
        writeFourDigitNumber(4, dataOut[1]);   // Display oil temperature
        writeFourDigitNumber(8, dataOut[2]);   // Display oil pressure
    }
}
