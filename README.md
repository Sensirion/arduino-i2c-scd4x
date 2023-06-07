# SCD4x Library

This is a library to interface with the Sensirion SCD4x true CO2 sensors in Arduino using the I2C protocol.

## Warning
These sensors by default have an auto-calibrate mode that takes the lowest CO2 level from the last week and assumes it is 400ppm. This can cause the sensor to read hundreds of ppm off if it is in a room that doesn't get to 400ppm of CO2 in a week. It's important to be aware of this issue, as some scientific papers are even using this sensor and haven't noticed the problem. Here is the code that you need to permanently set them to not auto calibrate. To avoid unnecessary wear of the EEPROM, the saveSettings command should only be used sparingly. The EEPROM is guaranteed to endure at least 2000 write cycles before failure.

```c++
#include "scd4x.h"

Wire.begin();
co2.begin(Wire);
co2.setCalibrationMode(false);
co2.saveSettings();
```

Despite this issue with the auto-calibration mode, I still believe that the Sensirion SCD4x CO2 Sensors are a great choice for measuring indoor air quality. In my experience, they have proven to be much more accurate than other popular sensors such as eCO2 sensors. It's important to be aware of this particular limitation and take the necessary steps to disable the auto-calibration mode, but overall, these sensors are a reliable and effective tool for monitoring CO2 levels.

## Factory Calibration with auto-calibration off
![Co Location Calibration](/images/cal.png)

## Features
- use multiple I2C Busses -> scd4x.begin(Wire1);
- no extra dependencies
- only implements necessary functions
- uses doubles (64bit floating point numbers) for proper accurate data calculations

## Warnings
- not all functions are implemented
- not compatible with other SCD4x Arduino libraries
- only tested with the esp32

### Setup
```c++
#include "scd4x.h"
SCD4X co2;
double CO2 = 0, temperature = 0, humidity = 0;

Wire.begin();
co2.begin(Wire);
co2.startPeriodicMeasurement();
```
### Loop
```c++
while (co2.isDataReady() == false) {
	vTaskDelay(50 / portTICK_PERIOD_MS); //check every 50ms
}

if (co2.readMeasurement(CO2, temperature, humidity) == 0) {
	Serial.printf("%4.0f,%2.1f,%1.0f\n", CO2, temperature, humidity); //nice formatting of data
}
vTaskDelay(4750 / portTICK_PERIOD_MS); //new data available after approx 5 seconds
```

## 🖼️ Schematic
![Schematic](/images/schematic.png)
- the scd4x sensor can draw up to 205ma peaks at 3.3V (only 18ma average) so make sure you have a robust power source (the above schematic has been tested to work)
- you only need to solder the 6 pins shown and the thermal pad on the underside to get it working
- unfortunately, I have not found a way to easily solder these sensors with a soldering iron, a oven or hotplate seems to be the only way
- look out for a temperature offset if you place the sensor in a case of sorts

## Based on the awesome work of Raphael Nestler and everyone at Sensirion AG
Origin created by Raphael Nestler in 2021
https://github.com/Sensirion/arduino-i2c-scd4x


To help support my work check out my store: https://keastudios.co.nz/
