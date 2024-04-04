#include "SensorModule.h"   // Include sensor module header file
#include "LedControl.h"     // Include LED control library

LedControl lc = LedControl(11, 13, 12, 8);  // Initialize LED control pins
#define ovenTempPin A0       // Define pin for oven temperature sensor
#define oilTempPin A1        // Define pin for oil temperature sensor
#define oilPressurePin A2    // Define pin for oil pressure sensor

#define ovenTempOffset 0
#define oilTempOffset 0
#define oilPressureOffset 0

void initializeSensorModule() {
    pinMode(ovenTempPin, INPUT);        // Set oven temperature pin as input
    pinMode(oilTempPin, INPUT);         // Set oil temperature pin as input
    pinMode(oilPressurePin, INPUT);     // Set oil pressure pin as input

    int deviceCount = lc.getDeviceCount();
    for (int address = 0; address < deviceCount; address++) {
        lc.shutdown(address, false);
        lc.setIntensity(address, 8);
        lc.clearDisplay(address);
    }
}

void readSensorData(int dataOut[]) {
    // Read sensor data and map to meaningful values
    dataOut[0] = map(analogRead(ovenTempPin), 0, 1023, -100, 450) + ovenTempOffset;        // Read oven temperature
    dataOut[1] = map(analogRead(oilTempPin), 0, 1023, -100, 450) + oilTempOffset;         // Read oil temperature
    dataOut[2] = abs(map(analogRead(oilPressurePin), 204, 1023, 0, 500)) + oilPressureOffset;     // Read oil pressure
}

void writeFourDigitNumber(int offset, int n) {
    bool wasNegative = n < 0;

    n = abs(n);

    int digit = n % 10;  // Get the least significant digit
    int truncatedOffset = offset % 8;   // Truncate offset to fit within 8 digits
    for (int i = 0; i < 4; i++) {
        lc.setDigit(floor(offset / 8), truncatedOffset + i, digit, false);  // Write digit to LED display
        n = n / 10;         // Move to the next digit
        digit = n % 10;     // Get the next digit
    }

    // offset check is since oil pressure cannot be negative
    if (wasNegative && offset != 0) {
      lc.setChar(floor(offset / 8), truncatedOffset + 5, '-', false);
    } else {
      lc.setChar(floor(offset / 8), truncatedOffset + 5, ' ', false);
    }
}

void sendData(int dataOut[]) {
    // Print sensor data over serial
    Serial.print("Oven Temp: ");
    Serial.println(dataOut[0]);
    Serial.print("Oil Temp: ");
    Serial.println(dataOut[1]);
    Serial.print("Oil Pressure: ");
    Serial.println(dataOut[2]);
}

void updateDisplay(int dataOut[]) {
    writeFourDigitNumber(16, dataOut[0]);   // Display oven temperature
    writeFourDigitNumber(8, dataOut[1]);   // Display oil temperature
    writeFourDigitNumber(0, dataOut[2]);   // Display oil pressure
}
