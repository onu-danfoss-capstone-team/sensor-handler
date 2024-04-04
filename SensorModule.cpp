#include "SensorModule.h"  // Include sensor module header file

#include "LedControl.h"  // Include LED control library

LedControl lc = LedControl(11, 13, 12, 8);  // Initialize LED control pins
#define ovenTempPin A0     // Define pin for oven temperature sensor
#define oilTempPin A1      // Define pin for oil temperature sensor
#define oilPressurePin A2  // Define pin for oil pressure sensor

void initializeSensorModule() {
  pinMode(ovenTempPin, INPUT);     // Set oven temperature pin as input
  pinMode(oilTempPin, INPUT);      // Set oil temperature pin as input
  pinMode(oilPressurePin, INPUT);  // Set oil pressure pin as input

  int deviceCount = lc.getDeviceCount();
  for (int address = 0; address < deviceCount; address++) {
    lc.shutdown(address, false);
    lc.setIntensity(address, 8);
    lc.clearDisplay(address);
  }
}

void readSensorData(long int dataOut[], bool testEnabled) {
  // Read sensor data and map to meaningful values
  if (testEnabled) {
    dataOut[0] += map(analogRead(ovenTempPin), 0, 1023, -100, 450);
    dataOut[1] += map(analogRead(oilTempPin), 0, 1023, -100, 450);
    dataOut[2] += map(analogRead(oilPressurePin), 204, 1023, 0, 500);
  } else {
    dataOut[0] += map(analogRead(ovenTempPin), 0, 1023, -100, 450) * 0.84;
    dataOut[1] += map(analogRead(oilTempPin), 0, 1023, -100, 450) * 0.84;
    dataOut[2] += map(analogRead(oilPressurePin), 204, 1023, 0, 500) * 0.84;
  }
}

void writeFourDigitNumber(long int offset, int n) {
  bool wasNegative = n < 0;

  n = abs(n);

  int digit = n % 10;                // Get the least significant digit
  int truncatedOffset = offset % 8;  // Truncate offset to fit within 8 digits
  for (int i = 0; i < 4; i++) {
    lc.setDigit(floor(offset / 8), truncatedOffset + i, digit,
                false);  // Write digit to LED display
    n = n / 10;          // Move to the next digit
    digit = n % 10;      // Get the next digit
  }

  // offset check is since oil pressure cannot be negative
  if (wasNegative && offset != 0) {
    lc.setChar(floor(offset / 8), truncatedOffset + 5, '-', false);
  } else {
    lc.setChar(floor(offset / 8), truncatedOffset + 5, ' ', false);
  }
}

void sendData(long int dataOut[], long int readingsSinceLast) {
  // Print sensor data over serial
  // Serial.print("Oven Temp: ");
  Serial.print(readingsSinceLast);
  Serial.print(", ");
  Serial.print(dataOut[0] / readingsSinceLast);
  Serial.print(", ");
  // Serial.print("Oil Temp: ");
  Serial.print(dataOut[1] / readingsSinceLast);
  Serial.print(", ");
  // Serial.print("Oil Pressure: ");
  Serial.println(dataOut[2] / readingsSinceLast);
}

void updateDisplay(long int dataOut[], long int readingsSinceLast) {
  writeFourDigitNumber(
      16, dataOut[0] / readingsSinceLast);  // Display oven temperature
  writeFourDigitNumber(
      8, dataOut[1] / readingsSinceLast);  // Display oil temperature
  writeFourDigitNumber(0,
                       dataOut[2] / readingsSinceLast);  // Display oil pressure
}
