# SCD4x CO2 Library

The SCD4x Library provides an interface to interact with Sensirion SCD4x true CO2 sensors using the I2C protocol in Arduino.

## Important Notice: Auto-Calibration Issue and Recommended Actions
---------------------------------------------------------------

When using the SCD4x CO2 sensor, it is essential to be aware of the auto-calibration issue. The default calibration mode assumes a CO2 level of 400ppm, which may not accurately reflect the actual environmental conditions. This can lead to inaccurate readings.

To address this issue, we recommend disabling the auto-calibration mode by using the provided code snippet:

```c++
#include "scd4x.h"
SCD4X co2;
void setup() {
  co2.begin();
  co2.setCalibrationMode(false);
  co2.saveSettings();
}
```
Please note that exercising caution when using the `saveSettings()` command, as it writes to the EEPROM, which has a limited lifespan of approximately 2000 write cycles before potential failure.

## Factory Calibration with Auto-Calibration Off
--------------------------------------------

![Co Location Calibration](/images/cal.png)

## Features
---------

* Supports multiple I2C busses: `scd4x.begin(Wire1);`
* No extra dependencies required
* Implements only necessary functions
* Uses doubles (64-bit floating-point numbers) for accurate data calculations

## Warnings
---------

- Not all functions are implemented
- Not compatible with other SCD4x Arduino libraries
- Only tested with the ESP32

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
} vTaskDelay(4750 / portTICK_PERIOD_MS); //new data available after approx 5 seconds
```

## Schematic
------------

![Schematic](/images/schematic.png)

* The SCD4x sensor can draw up to 205mA peaks at 3.3V (only 18mA average), so ensure you have a robust power source.
* You only need to solder the 6 shown pins and the thermal pad on the underside to get it working.
* Unfortunately, soldering these sensors with a soldering iron is not easy; an oven or hotplate seems to be the only way.
* Watch out for a temperature offset if you place the sensor in a case or enclosure.

## Function Reference
--------------------

| Function                     | Description                                                                                                                                                                                                                                                                                  |
| ---------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `begin()`                    | Initializes the library. Must be called before any other library functions.                                                                                                                                                                                                                  |
| `isConnected()`              | Checks if the device is correctly connected by verifying the response, manufacturer ID, and part ID.                                                                                                                                                                                         |
| `startPeriodicMeasurement()` | Starts periodic measurement, with new data available in approximately 5 seconds.                                                                                                                                                                                                             |
| `stopPeriodicMeasurement()`  | Stops periodic measurement. Wait at least 500ms before sending further commands.                                                                                                                                                                                                             |
| `readMeasurement()`          | Reads the sensor output, including CO₂ concentration in ppm, temperature in °C, and relative humidity in %RH. The measurement data can only be read out once per signal update interval as the buffer is emptied upon read-out.                                                              |
| `isDataReady()`              | Checks whether new measurement data is available for read-out.                                                                                                                                                                                                                               |
| `setCalibrationMode()`       | Sets the calibration mode and stores it in the EEPROM of the SCD4x. The automatic self-calibration algorithm assumes that the sensor is exposed to the atmospheric CO2 concentration of 400 ppm at least once per week. Use this function sparingly to avoid unnecessary wear of the EEPROM. |
| `getCalibrationMode()`       | Gets the calibration mode. Returns `true` if auto calibration is enabled, `false` otherwise.                                                                                                                                                                                                 |
| `saveSettings()`             | Stores settings in the EEPROM of the SCD4x. Wait at least 800ms before sending further commands. EEPROM is guaranteed to endure at least 2000 write cycles before failure.                                                                                                                   |
| `getErrorText()`             | Converts an error code into descriptive text. Returns a pointer to a constant character array containing the descriptive text of the error. If the error code is not recognized, "Unknown error" is returned.                                                                                |

## Credits
---------

Based on the work of Raphael Nestler and everyone at Sensirion AG.

Originally created by Raphael Nestler in 2021.

https://github.com/Sensirion/arduino-i2c-scd4x

To help support my work, check out my store: https://keastudios.co.nz/
