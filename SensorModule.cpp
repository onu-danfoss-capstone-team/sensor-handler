#include "SensorModule.h"  // Include sensor module header file

#include "LedControl.h"  // Include LED control library

LedControl lc = LedControl(11, 13, 12, 8);  // Initialize LED control pins

#define OVEN_TEMP_PIN A0     // Define pin for oven temperature sensor
#define OIL_TEMP_PIN A1      // Define pin for oil temperature sensor
#define OIL_PRESSURE_PIN A2  // Define pin for oil pressure sensor

float ovenTempSlope = (1023 - 0) / (450 + 100);
float ovenTempOffset = 0.0;
float oilTempSlope = (1023 - 0) / (450 + 100);
float oilTempOffset = 0.0;
float oilPressureSlope = (1023 - 204) / (450 + 100);
float oilPressureOffset = 0.0;

int vars[12] = {0};

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
                     int expected1, int expected2) {
  slope = (expected2 - expected1) / (float) (raw2 - raw1);
  offset = expected1 - slope * raw1;
}

void calibrateSensors() {
  for (int i = 0; i<12; i++) {
    if (Serial.available() > 0) {
      int val = Serial.parseInt();
      if (val == 0) {
        continue;
      }
      vars[i] = val;
    }
  }

  for (int i = 0; i<12; i++) {
    Serial.print(vars[i]);
    Serial.print(',');
  }
  Serial.println('\n');

  // Perform calibration
  calibrateSensor(ovenTempSlope, ovenTempOffset, vars[0], vars[1],
                  vars[2], vars[3]);
  calibrateSensor(oilTempSlope, oilTempOffset, vars[4], vars[5],
                  vars[6], vars[7]);
  calibrateSensor(oilPressureSlope, oilPressureOffset, vars[8],
                  vars[9], vars[10], vars[11]);
}

void readSensorData(long int dataOut[], bool testEnabled) {
  int rawOvenTemp = analogRead(OVEN_TEMP_PIN);
  int rawOilTemp = analogRead(OIL_TEMP_PIN);
  int rawOilPressure = analogRead(OIL_PRESSURE_PIN);

  // Read sensor data and map to meaningful values
  if (testEnabled) {
    dataOut[0] += rawOvenTemp;
    dataOut[1] += rawOilTemp;
    dataOut[2] += rawOilPressure;
  } else {
    //Serial.println("----------OVEN---------");
    //Serial.println(rawOvenTemp);
    //Serial.println(ovenTempSlope);
    //Serial.println(ovenTempOffset);
    //Serial.println("----------OIL---------");
    //Serial.println(rawOilTemp);
    //Serial.println(oilTempSlope);
    //Serial.println(oilTempOffset);
    //Serial.println("---------------------");
    float adjustedOvenTemp = (rawOvenTemp * ovenTempSlope) + ovenTempOffset;
    float adjustedOilTemp = (rawOilTemp * oilTempSlope) + oilTempOffset;
    float adjustedOilPressure =
        (rawOilPressure * oilPressureSlope) + oilPressureOffset;

    dataOut[0] += adjustedOvenTemp;
    dataOut[1] += adjustedOilTemp;
    dataOut[2] += adjustedOilPressure;
  }
}

void writeFourDigitNumber(long int offset, int n, bool testEnabled) {
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

  if (testEnabled) {
    lc.setChar(floor(offset / 8), truncatedOffset + 7, 'C', false);
  } else {
    lc.setChar(floor(offset / 8), truncatedOffset + 7, ' ', false);
  }
}

void sendData(long int dataOut[], long int readingsSinceLast, bool testEnabled) {
  if (testEnabled) {
    return;
  }
  Serial.print(dataOut[0] / readingsSinceLast);
  Serial.print(", ");
  Serial.print(dataOut[1] / readingsSinceLast);
  Serial.print(", ");
  Serial.println(dataOut[2] / readingsSinceLast);
}

void updateDisplay(long int dataOut[], long int readingsSinceLast, bool testEnabled) {
  writeFourDigitNumber(
      16, dataOut[0] / readingsSinceLast, testEnabled);  // Display oven temperature
  writeFourDigitNumber(
      8, dataOut[1] / readingsSinceLast, testEnabled);  // Display oil temperature
  writeFourDigitNumber(0,
                       dataOut[2] / readingsSinceLast, testEnabled);  // Display oil pressure
}
