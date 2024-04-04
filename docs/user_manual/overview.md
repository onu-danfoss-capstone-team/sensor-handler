# Overview

The Danfoss Capstone Lab Digitization Records System utilizes an AtMega 328PU microcontroller with 10 bit analog to digital conversion capabilities. Two signal conditioners are used to convert the small voltage produced by the thermocouples to 0-5V usable by the microcontroller. The ADC outputs a number between 0-1023 which is converted from levels to degrees Celsius by the microcontroller before being output to the seven-segment displays. The pressure sensor outputs 1-5V which is also read and output in the same manner.

The data rate is set by the user. A serial cable is connected to a computer running MS Excel and at the intervals set by the user, data is exported via the serial cable and logged by the software. The data logger in Excel uses a standard spreadsheet, so if technicians need to leave notes, they may do so in the cells next to the data.

Once the data is logged and the test is complete, the data may be exported as a spreadsheet or copy-pasted into a report as needed.
