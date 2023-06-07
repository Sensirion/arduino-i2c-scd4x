/*
  SCD4x CO2 Sensor Example

  This example demonstrates the usage of the SCD4x CO2 sensor library on an Arduino board.

  Instructions:
  - Install the SCD4x library in your Arduino IDE.
  - Connect the SCD4X CO2 sensor to the following pins on your Arduino board:
	- SDA: Default SDA pin
	- SCL: Default SCL pin
	- VCC: 3.3V or 5V (make sure you match this with your Arduino VCC voltage or otherwise you can damage it)
  - Optionally, you can add external 4.7k pull-up resistors on the SDA and SCL lines.
  - Flash code
  - Open the Serial Monitor in the Arduino IDE to view the measurement data and any error messages.

  Default I2C pins for some common Arduino boards:
  - Arduino Uno, Nano: SDA - A4, SCL - A5 (VCC: 5V)
  - Arduino Mega: SDA - 20, SCL - 21 (VCC: 5V)
  - Arduino Leonardo: SDA - 2, SCL - 3 (VCC: 5V)
  - Arduino Due: SDA - 20, SCL - 21 (VCC: 3.3V)
  - ESP32: SDA - GPIO 21, SCL - GPIO 22 (VCC: 3.3V)
  - ESP8266: SDA - GPIO 4, SCL - GPIO 5 (VCC: 3.3V)
  - RP2040: SDA - GP5 (pin 4), SCL - GP4 (pin 3) (VCC: 3.3V)

  Note: The default SDA and SCL pins may vary depending on the specific board or variant. Please refer to the board documentation for the correct pin mappings.

  4.7k Pull-up Resistors:
  - I2C communication requires pull-up resistors on the SDA and SCL lines to ensure proper signal levels.
  - Some Arduino boards already have built-in pull-up resistors for the I2C pins, so external resistors may not be necessary.
  - However, if you encounter communication issues or have long wire lengths, adding external 4.7k pull-up resistors between the SDA/SCL lines and VCC can help improve signal stability.
*/

#include <Wire.h>
#include <scd4x.h>

SCD4X co2;

void setup() {
	// Initialize the I2C communication
	Wire.begin();

	// Initialize the SCD4x library
	co2.begin();

	// Check if the sensor is connected
	if (!co2.isConnected()) {
		while (true) {
			Serial.println("Sensor not connected. Please check the wiring.");
			delay(1000);
		}
	}

	// Check if auto-calibration is enabled
	if (co2.getCalibrationMode()) {
		// Disable auto-calibration
		co2.setCalibrationMode(false);

		// Save the settings to EEPROM
		co2.saveSettings();
	}

	// Start periodic measurement after updating settings
	co2.startPeriodicMeasurement();

	// Wait for the sensor to warm up and take the first reading
	delay(5000);
}

void loop() {
	double co2Value, temperature, humidity;

	// Read measurement data from the sensor
	uint8_t errorCode = co2.readMeasurement(co2Value, temperature, humidity);

	// Check for errors
	if (errorCode == 0) {
		// Format the measurements into a string
		char measurementString[100];
		sprintf(measurementString, "CO2: %.0f ppm, Temperature: %.1f °C, Humidity: %.0f %%RH", co2Value, temperature, humidity);

		// Print the measurements
		Serial.println(measurementString);
	} else {
		// Convert the error code to text
		const char* errorText = co2.getErrorText(errorCode);

		// Print the error message
		Serial.print("Error reading measurement: ");
		Serial.println(errorText);
	}

	// Delay for new measurement to be taken
	delay(5000);
}
