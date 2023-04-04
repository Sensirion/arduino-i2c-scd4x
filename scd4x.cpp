/*
 * Copyright (c) 2021, Sensirion AG
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
	_i2cPort->beginTransmission(_address);
	return _i2cPort->endTransmission();
}

bool SCD4X::isConnected(TwoWire& port, Stream* stream, uint8_t addr) {
	_i2cPort = &port;
	_address = addr;

	_debug_output_stream = stream;

	_i2cPort->beginTransmission(_address);
	_error = _i2cPort->endTransmission(true);

	char addrCheck[32];
	if (_error != 0) {
		_debug_output_stream->printf("SCD4x returned endTransmission error %i\r\n", _error);
		return false;
	}

	_i2cPort->beginTransmission(_address);
	_i2cPort->write(0x36);
	_i2cPort->write(0x39);
	_i2cPort->endTransmission(true);

	vTaskDelay(10000 / portTICK_PERIOD_MS);

	uint8_t temp[3];
	if (_i2cPort->requestFrom(_address, (uint8_t)3)) {
		Wire.readBytes(temp, 3);
	}

	if (temp[0] != 0 || temp[1] != 0 || temp[2] != 0x81) {
		_debug_output_stream->printf("SCD4x returned selfTest Error: 0x%02X 0x%02X with CRC of 0x%02X\r\n", temp[0], temp[1], temp[2]);
		return false;
	}

	_debug_output_stream->printf("SCD4x Connected Correctly\r\n");
	return true;
}

uint8_t SCD4X::startPeriodicMeasurement() {
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(0x21);
	_i2cPort->write(0xb1);
	return _i2cPort->endTransmission();
}

uint8_t SCD4X::readMeasurement(double& co2, double& temperature, double& humidity) {
	const int bytesRequested = 12;

	_i2cPort->beginTransmission(_address);
	_i2cPort->write(0xec);
	_i2cPort->write(0x05);

	if (_i2cPort->endTransmission(true) == 0) {
		// read measurement data: 2 bytes co2, 1 byte CRC,
		// 2 bytes T, 1 byte CRC, 2 bytes RH, 1 byte CRC,
		// 2 bytes sensor status, 1 byte CRC
		// stop reading after bytesRequested (12 bytes)

		uint8_t bytesReceived = Wire.requestFrom(_address, bytesRequested);
		if (bytesReceived == bytesRequested) {	// If received requested amount of bytes
			uint8_t data[bytesReceived];
			Wire.readBytes(data, bytesReceived);

			// floating point conversion
			co2 = (double)((uint16_t)data[0] << 8 | data[1]);
			// convert T in degC
			temperature = (double)-45 + (double)175 * (double)((uint16_t)data[3] << 8 | data[4]) / (double)65536;
			// convert RH in %
			humidity = (double)100 * (double)((uint16_t)data[6] << 8 | data[7]) / (double)65536;

			if (inRange(co2, 40000, 0) && inRange(temperature, 60, -10) &&
				inRange(humidity, 100, 0)) {
				return false;
			} else {
				ESP_LOGE("measurement", "out of range");
				return true;
			}

		} else {
			//ESP_LOGE("Wire.requestFrom", "bytesReceived(%i) != bytesRequested(%i)", bytesReceived, bytesRequested);
			return true;
		}
	} else {
		ESP_LOGE("Wire.endTransmission", "Returned ERROR");
		return true;
	}
}

bool SCD4X::isDataReady() {
	const int bytesRequested = 3;

	_i2cPort->beginTransmission(_address);
	_i2cPort->write(0xe4);
	_i2cPort->write(0xb8);

	if (_i2cPort->endTransmission(true) == 0) {
		uint8_t bytesReceived = Wire.requestFrom(_address, bytesRequested);
		if (bytesReceived == bytesRequested) {	// If received requested amount of bytes
			uint8_t data[bytesReceived];
			Wire.readBytes(data, bytesReceived);
			return data[1] != (0x00);  // check if last 8 bits are not 0

		} else {
			ESP_LOGE("Wire.requestFrom",
					 "bytesReceived(%i) != bytesRequested(%i)", bytesReceived,
					 bytesRequested);
			return false;
		}
	} else {
		ESP_LOGE("Wire.endTransmission", "Returned ERROR");
		return false;
	}
}