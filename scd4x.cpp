/*
 * Copyright (c) 2021, Sensirion AG
 * Copyright (c) 2023, Chris Dirks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <scd4x.h>

uint8_t SCD4X::begin(TwoWire& port, uint8_t addr) {
	_i2cPort = &port;
	_address = addr;

	// Begin I2C transmission with the sensor's address
	_i2cPort->beginTransmission(_address);

	// End I2C transmission and retrieve the error code
	_error = _i2cPort->endTransmission();

	return _error;
}

bool SCD4X::isConnected(TwoWire& port, Stream* stream, uint8_t addr) {
	const int bytesRequested = 3;

	_debug_output_stream = stream;

	SCD4X::begin(port, addr);

	SCD4X::stopPeriodicMeasurement();
	vTaskDelay(500 / portTICK_PERIOD_MS);  // wait for SCD4x to stop as per datasheet

	if (_error != 0) {
		_debug_output_stream->printf("SCD4x returned endTransmission error %i\r\n", _error);
		return false;
	}

	_commandSequence(0x3639);

	vTaskDelay(10000 / portTICK_PERIOD_MS);	 // wait for SCD4x to do a self test as per datasheet

	uint8_t temp[bytesRequested];
	if (_i2cPort->requestFrom(_address, bytesRequested)) {
		_i2cPort->readBytes(temp, bytesRequested);
	}

	if (temp[0] != 0 || temp[1] != 0 || temp[2] != 0x81) {
		_debug_output_stream->printf("SCD4x returned selfTest Error: 0x%02X 0x%02X with CRC of 0x%02X\r\n", temp[0], temp[1], temp[2]);
		return false;
	}

	_debug_output_stream->printf("SCD4x Connected Correctly\r\n");
	return true;
}

uint8_t SCD4X::stopPeriodicMeasurement() {
	_commandSequence(0x3f86);
	return _error;
}

uint8_t SCD4X::startPeriodicMeasurement() {
	_commandSequence(0x21b1);
	return _error;
}

uint8_t SCD4X::readMeasurement(double& co2, double& temperature, double& humidity) {
	const int bytesRequested = 9;

	_i2cPort->beginTransmission(_address);
	_i2cPort->write(0xec);
	_i2cPort->write(0x05);
	_error = _i2cPort->endTransmission(false);	// no stop bit

	if (_error == 0) {
		// read measurement data: 2 bytes co2, 1 byte CRC,
		// 2 bytes T, 1 byte CRC, 2 bytes RH, 1 byte CRC,
		// 2 bytes sensor status, 1 byte CRC
		// stop reading after bytesRequested (12 bytes)

		uint8_t bytesReceived = _i2cPort->requestFrom(_address, bytesRequested);
		if (bytesReceived == bytesRequested) {	// If received requested amount of bytes
			uint8_t data[bytesReceived];
			_i2cPort->readBytes(data, bytesReceived);

			// converter co2 ppm to floating point
			co2 = (double)((uint16_t)data[0] << 8 | data[1]);
			// convert to temperature in degC
			temperature = (double)-45 + (double)175 * (double)((uint16_t)data[3] << 8 | data[4]) / (double)65536;
			// convert to relative humidity to %
			humidity = (double)100 * (double)((uint16_t)data[6] << 8 | data[7]) / (double)65536;

			// Check if measurements are within range
			if (inRange(co2, 40000, 0) && inRange(temperature, 60, -10) &&
				inRange(humidity, 100, 0)) {
				return 0;
			} else {
				ESP_LOGE("measurement", "out of range");
				Serial.printf("%4.0f,%2.1f,%1.0f\n", co2, temperature, humidity);
				_error = 7;
			}

		} else {
			// ESP_LOGE("Wire.requestFrom", "bytesReceived(%i) != bytesRequested(%i)", bytesReceived, bytesRequested);
			_error = 6;
		}
	}
	return _error;
}

bool SCD4X::isDataReady() {
	if ((_readSequence(0xe4b8) & 0x07ff) == 0x0000) {  // lower 11 bits == 0 -> data not ready
		return false;
	} else {
		return true;
	}
}

bool SCD4X::getCalibrationMode() {
	return (bool)_readSequence(0x2313);
}

uint8_t SCD4X::setCalibrationMode(bool enableSelfCalibration) {
	SCD4X::stopPeriodicMeasurement();
	vTaskDelay(500 / portTICK_PERIOD_MS);  // wait for SCD4x to stop as per datasheet

	if (enableSelfCalibration) {
		SCD4X::_writeSequence(0x2416, 0x0001, 0xB0);
	} else {
		SCD4X::_writeSequence(0x2416, 0x0000, 0x81);
	}

	return _error;
}

uint8_t SCD4X::saveSettings() {
	_commandSequence(0x3615);
	ESP_LOGI("Settings Saved to EEPROM", "");
	vTaskDelay(800 / portTICK_PERIOD_MS);  // wait for SCD4x to saveSettings as per datasheet
	return _error;
}

const char* SCD4X::getErrorText(uint8_t errorCode) {
	switch (errorCode) {
		case 0:
			return "Success";
		case 1:
			return "I2C data too long to fit in transmit buffer";
		case 2:
			return "I2C received NACK on transmit of address";
		case 3:
			return "I2C received NACK on transmit of data";
		case 4:
			return "I2C other error";
		case 5:
			return "I2C timeout";
		case 6:
			return "bytesReceived(%i) != bytesRequested(%i)";
		case 7:
			return "Measurement out of range";
		default:
			return "Unknown error";
	}
}
