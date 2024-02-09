/*
Danfoss Capstone Group
Lab Records Digitization

This code is meant to read two thermocouples and a pressure sensor with 0-5V inputs (1-5V for the pressure). The analog voltage will be
converted to the appropriate units. This information will be output over serial and logged in a CSV file. A seven segment display will
also show realtime data.

Version 2 will allow Bluetooth communication between microcontrollers allowing wireless tranfer of the data to the computer. May not
need a second microcontroller if bluetooth dongles may be used instead. Need to look into this.

NOTE: This microcontroller must be reset every 49.7 days because of the limitations of the timing functions.

*/

#include "LedControl.h"

/*
ARGORDER:
DIN
CLK
LOAD
 ***** Please set the number of devices you have *****
 But the maximum default of 8 MAX72XX wil also work.
 */
LedControl lc = LedControl(11, 13, 12, 8);

// Define pins for the analog read
#define ovenTempPin A0    // Thermocouple for inside the oven
#define oilTempPin A1     // Thermocouple for the oil reservoir
#define oilPressurePin A2 // Pressure sensor, 0-5V output
#define startButton 5     // digital pin to accept user input to run the program.
// create the user-readable phrases that correspond to the test parameters.
const String ovenTemp = "Oven tempurature: ";
const String oilTemp = "Oil tempurature: ";
const String oilPressure = "Oil Pressure: ";

// The seven segment displays are daisy chained because they require the only MOSI and SCK pins on the Arduino. The only parameter
// that is user defined is the CS pin
#define CS 7

// Create the variables needed that are not part of the hardware:

double dataIntervalMin = .2;              // Set this to determine the data aquisition interval in minutes.
uint32_t dataIntervalMilli = 0;           // data aquisition interval in milliseconds. This should not be changed.
uint32_t currentTime = 0;                 // intialize a variable for the timing of sending the data. Do not change value.
uint32_t timeTrigger = 0;                 // this is the time in which each dataset is sent over serial.
uint16_t dataOut[6] = {0, 0, 0, 0, 0, 0}; // initialize an array so for the sensor values. Do not alter.
bool firstScan = 1;                       // Ensure the data is sent on the first scan

void writeFourDigitNumber(int offset, int n)
{
    int digit = n % 10;
    int truncatedOffset = offset % 8;
    for (int i = 0; i < 4; i++)
    {
        lc.setDigit(floor(offset / 8), truncatedOffset + i, digit, false);
        n = n / 10;
        digit = n % 10;
    }
}

void setup()
{
    setPinMode();                                         // This sets the microcontrollers pins to be inputs or outputs as needed. See below to modify.
    Serial.begin(9600);                                   // initialize the serial communication for computer or bluetooth. 9600 baud should be more than adequate.
    dataIntervalMilli = setDataInterval(dataIntervalMin); // convert the data aquisition time from minutes to milliseconds.
    Serial.print("Data interval set to ");
    Serial.print(dataIntervalMin);
    Serial.println(" minutes.");
    // this loop will pause the program giving the technician time to set up. When the button is pushed, the program will run on a loop until
    // stopped.
    while (digitalRead(startButton))
    {
    }
    // once the button is pressed, set the time for the data aquisition interval.
    Serial.println("Test Begun");
    uint8_t currentTime = millis();

    int devices = lc.getDeviceCount();
    for (int address = 0; address < devices; address++)
    {
        lc.shutdown(address, false);
        lc.setIntensity(address, 8);
        lc.clearDisplay(address);
    }
}

void loop()
{
    getData();
    sendData(dataIntervalMilli, dataOut);
    for (int i = 0; i < 6; i++)
    {
        writeFourDigitNumber(i * 4, dataOut[i]);
    }
}

// This is the initial setup of the microcontroller pins
// un-comment the pins needed in the setup
void setPinMode()
{
    // analog pins
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    // pinMode(A3, INPUT);
    // pinMode(A4, INPUT);
    // pinMode(A5, INPUT);

    // digital pins
    pinMode(CS, OUTPUT);
    pinMode(startButton, INPUT_PULLUP);
}

// Read and convert the inputs to appropriate units. Add analog read functions as necessary to accomodate other testing.

void getData()
{
    // each pin is read and converted from the analog voltage to the appropriate unit. These are set at factory claims.
    dataOut[0] = map(analogRead(ovenTempPin), 0, 1023, -50, 450);
    dataOut[1] = map(analogRead(oilTempPin), 0, 1023, -50, 450);
    dataOut[2] = map(analogRead(oilPressurePin), 204, 1023, 0, 500);
    // dataOut[] = map(analogRead(), , , , );
    // dataOut[] = map(analogRead(), , , , );
    // dataOut[] = map(analogRead(), , , , );
}

// This will allow the technician to easily set the data aquisition interval in minutes. Time between intervals is limited to
// less than 1200 hours.

double setDataInterval(double timeInMin)
{
    double timeInMilliseconds = timeInMin * 60.0 * 1000.0;
    return timeInMilliseconds;
}

// Write the data values to the serial port once every data aquisition interval. Commas between the datapoints sends the next
// data to the following cell in Excel.

void sendData(double dataInterval, uint16_t data[6])
{
    // only print the values at the proper time interval.
    currentTime = millis();
    // make sure you take out the strings before using with Excel
    if (currentTime >= timeTrigger + dataInterval)
    {
        timeTrigger = millis();
        Serial.print("Oven Temp: ");
        Serial.print(data[0]);
        Serial.print(", ");
        Serial.print("Oil Temp: ");
        Serial.print(data[1]);
        Serial.print(", ");
        Serial.print("Oil Pressure: ");
        Serial.println(data[2]);
        // Serial.println(",");
        // delay(1000);
        // Serial.println(ovenTemp + data[0]);
        // Serial.println(oilTemp + data[1]);
        // Serial.println(oilPressure + data[2]);
        // Serial.println(oilPressure + input3);
        // Serial.println(oilPressure + input3);
        // Serial.println(oilPressure + input3);
    }

    else if (firstScan)
    {
        Serial.print(data[0]);
        Serial.print(",");
        Serial.print(data[1]);
        Serial.print(",");
        Serial.println(data[2]);
        // Serial.println(",");
        // Serial.println(ovenTemp + data[0]);
        // Serial.println(oilTemp + data[1]);
        // Serial.println(oilPressure + data[2]);
        // Serial.println(oilPressure + input3);
        // Serial.println(oilPressure + input3);
        // Serial.println(oilPressure + input3);
        firstScan = 0;
    }
}
