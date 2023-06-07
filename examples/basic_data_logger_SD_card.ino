#include <SD.h>
#include <Wire.h>
#include <scd4x.h>

SCD4X co2;

const int chipSelectPin = 10;			   // Chip select pin for SD card module
const String logFileName = "co2_log.txt";  // Name of the log file

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		// Wait for serial port to connect
		delay(100);
	}

	// Initialize SD card
	if (!SD.begin(chipSelectPin)) {
		Serial.println("SD card initialization failed.");
		return;
	}

	// Initialize the I2C communication
	Wire.begin();

	// Initialize the SCD4x library
	if (!co2.begin()) {
		Serial.println("Failed to initialize SCD4X sensor.");
		return;
	}

	// Check if the sensor is connected
	if (!co2.isConnected()) {
		Serial.println("Sensor not connected. Please check the wiring.");
		return;
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

	// Create or append to the log file
	File logFile = SD.open(logFileName, FILE_WRITE);
	if (logFile) {
		logFile.println("CO2 (ppm), Temperature (°C), Humidity (%RH)");
		logFile.close();
		Serial.println("Data logging started. Press reset to start a new log.");
	} else {
		Serial.println("Failed to open log file.");
		return;
	}
}

void loop() {
	double co2Value, temperature, humidity;

	// Read measurement data from the sensor
	uint8_t errorCode = co2.readMeasurement(co2Value, temperature, humidity);

	// Check for errors
	if (errorCode == 0) {
		// Format the measurements into a string
		char measurementString[50];
		snprintf(measurementString, sizeof(measurementString), "%.0f, %.1f, %.0f", co2Value, temperature, humidity);

		// Open the log file in append mode
		File logFile = SD.open(logFileName, FILE_WRITE | FILE_APPEND);
		if (logFile) {
			// Write the measurement data to the log file
			logFile.println(measurementString);
			logFile.close();
			Serial.println("Data logged: " + String(measurementString));
		} else {
			Serial.println("Failed to open log file.");
		}

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