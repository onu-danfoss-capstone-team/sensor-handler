#include "SensorModule.h"  // Include sensor module header file

#include "LedControl.h"  // Include LED control library

LedControl lc = LedControl(11, 13, 12, 8);  // Initialize LED control pins

#define OVEN_TEMP_PIN A0     // Define pin for oven temperature sensor
#define OIL_TEMP_PIN A1      // Define pin for oil temperature sensor
#define OIL_PRESSURE_PIN A2  // Define pin for oil pressure sensor

float ovenTempSlope = 1.0;
float ovenTempOffset = 0.0;
float oilTempSlope = 1.0;
float oilTempOffset = 0.0;
float oilPressureSlope = 1.0;
float oilPressureOffset = 0.0;

void initializeSensorModule() {
  pinMode(OVEN_TEMP_PIN, INPUT);     // Set oven temperature pin as input
  pinMode(OIL_TEMP_PIN, INPUT);      // Set oil temperature pin as input
  pinMode(OIL_PRESSURE_PIN, INPUT);  // Set oil pressure pin as input

  int deviceCount = lc.getDeviceCount();
  for (int address = 0; address < deviceCount; address++) {
    lc.shutdown(address, false);
    lc.setIntensity(address, 8);
    lc.clearDisplay(address);
  }
}

void calibrateSensor(float &slope, float &offset, int raw1, int raw2,
                     float expected1, float expected2) {
  slope = (expected2 - expected1) / (raw2 - raw1);
  offset = expected1 - slope * raw1;
}

void calibrateSensors() {
  // Raw and expected values for oven temperature
  int ovenTempRaw1 = Serial.parseInt();
  int ovenTempRaw2 = Serial.parseInt();
  float ovenTempExpected1 = Serial.parseInt();
  float ovenTempExpected2 = Serial.parseInt();

  // Raw and expected values for oil temperature
  int oilTempRaw1 = Serial.parseInt();
  int oilTempRaw2 = Serial.parseInt();
  float oilTempExpected1 = Serial.parseInt();
  float oilTempExpected2 = Serial.parseInt();

  // Raw and expected values for oil pressure
  int oilPressureRaw1 = Serial.parseInt();
  int oilPressureRaw2 = Serial.parseInt();
  float oilPressureExpected1 = Serial.parseInt();
  float oilPressureExpected2 = Serial.parseInt();

  // Perform calibration
  calibrateSensor(ovenTempSlope, ovenTempOffset, ovenTempRaw1, ovenTempRaw2,
                  ovenTempExpected1, ovenTempExpected2);
  calibrateSensor(oilTempSlope, oilTempOffset, oilTempRaw1, oilTempRaw2,
                  oilTempExpected1, oilTempExpected2);
  calibrateSensor(oilPressureSlope, oilPressureOffset, oilPressureRaw1,
                  oilPressureRaw2, oilPressureExpected1, oilPressureExpected2);
}

void readSensorData(long int dataOut[], bool testEnabled) {
  int rawOvenTemp = analogRead(ovenTempPin);
  int rawOilTemp = analogRead(oilTempPin);
  int rawOilPressure = analogRead(oilPressurePin);

  // Read sensor data and map to meaningful values
  if (testEnabled) {
    dataOut[0] += rawOvenTemp;
    dataOut[1] += rawOilTemp;
    dataOut[2] += rawOilPressure;
  } else {
    float adjustedOvenTemp = (rawOvenTemp * ovenTempSlope) + ovenTempOffset;
    float adjustedOilTemp = (rawOilTemp * oilTempSlope) + oilTempOffset;
    float adjustedOilPressure =
        (rawOilPressure * oilPressureSlope) + oilPressureOffset;

    dataOut[0] += adjustedOvenTemp;
    dataOut[1] += adjustedOilTemp;
    dataOut[2] += adjustedOilPressure;
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
