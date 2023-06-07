/*
  SCD4X Measurement Task Example

  This example demonstrates the usage of the SCD4X CO2 sensor library with FreeRTOS on an Arduino board,
  specifically designed for the ESP32's version of Arduino with FreeRTOS.

  Instructions:
  - Install the SCD4X library in your Arduino IDE or add it to platformio.ini
  - Connect the SCD4X CO2 sensor to the ESP32:
	- SDA: GPIO 21
	- SCL: GPIO 22
	- VCC: 3V3 (3.3V)
  - Optionally, you can add external 4.7k pull-up resistors on the SDA and SCL lines.
  - Flash code
  - Open the Serial Monitor in the Arduino IDE to view the measurement data.

  Note: This example is specifically designed for the ESP32's version of Arduino with FreeRTOS.
*/

#include <scd4x.h>

SCD4X co2;
double co2Value = 0, temperature = 0, humidity = 0;
TaskHandle_t measurementTask;

void measurementTaskFunction(void* parameter) {
	Wire.begin();  // SDA: GPIO 21, SCL: GPIO 22 by default on the ESP32
	co2.begin(Wire);
	co2.startPeriodicMeasurement();

	while (true) {
		if (co2.isDataReady()) {
			if (co2.readMeasurement(co2Value, temperature, humidity) == 0) {
				Serial.printf("CO2: %.0f ppm, Temperature: %.1f °C, Humidity: %.0f %%RH\n", co2Value, temperature, humidity);
				vTaskDelay(pdMS_TO_TICKS(4750));  // New data available after approximately 5 seconds
			}
		}
		vTaskDelay(pdMS_TO_TICKS(50));	// Check every 50ms
	}
}

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		// Wait for serial port to connect
		delay(100);
	}

	xTaskCreatePinnedToCore(measurementTaskFunction, "MeasurementTask", 2048, NULL, 1, &measurementTask, 0);
}

void loop() {
	// Your other code here
}
