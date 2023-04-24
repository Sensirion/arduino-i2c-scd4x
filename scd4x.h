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

#include <Arduino.h>
#include <Wire.h>

#define SCD4X_I2C_ADDRESS 0x62	// 7-bit I2C Address

class SCD4X {
   public:
	/**
	 * begin() - Initializes this library. Must be called before any other library functions
	 * @note this function does not start the i2c bus, you must do that before calling this command
	 *
	 * @param i2cBus Arduino stream object to use for communication.
	 *
	 * @retval 0 success
	 * @retval 1 i2c data too long to fit in transmit buffer
	 * @retval 2 i2c received NACK on transmit of address
	 * @retval 3 i2c received NACK on transmit of data
	 * @retval 4 i2c other error
	 * @retval 5 i2c timeout
	 */
	uint8_t begin(TwoWire& port = Wire, uint8_t addr = SCD4X_I2C_ADDRESS);

	/**
	 * checks for correct response, manufacturer id and part id.
	 *
	 * @param port Wire instance (e.g Wire or Wire1)
	 * @param stream debug output pointer (e.g. &Serial)
	 * @param addr i2c address of sensor (0x62 by default)
	 * @returns true if device correctly connected, otherwise false
	 */
	bool isConnected(TwoWire& port = Wire, Stream* stream = &Serial, uint8_t addr = SCD4X_I2C_ADDRESS);

	/**
	 * startPeriodicMeasurement() - start periodic measurement, new data available
	 * in ~5 seconds.
	 *
	 * @retval 0 success
	 * @retval 1 i2c data too long to fit in transmit buffer
	 * @retval 2 i2c received NACK on transmit of address
	 * @retval 3 i2c received NACK on transmit of data
	 * @retval 4 i2c other error
	 * @retval 5 i2c timeout
	 */
	uint8_t startPeriodicMeasurement();

	/**
	 * stopPeriodicMeasurement() - stop periodic measurement
	 * @note wait atleast 500ms before sending further commands
	 *
	 * @retval 0 success
	 * @retval 1 i2c data too long to fit in transmit buffer
	 * @retval 2 i2c received NACK on transmit of address
	 * @retval 3 i2c received NACK on transmit of data
	 * @retval 4 i2c other error
	 * @retval 5 i2c timeout
	 */
	uint8_t stopPeriodicMeasurement();

	/**
	 * readMeasurement() - read sensor output. The measurement data can
	 * only be read out once per signal update interval as the buffer is emptied
	 * upon read-out. If no data is available in the buffer, the sensor returns
	 * a NACK. To avoid a NACK response the get_data_ready_status can be issued
	 * to check data status. The I2C master can abort the read transfer with a
	 * NACK followed by a STOP condition after any data byte if the user is not
	 * interested in subsequent data.
	 *
	 * @note This command is only available in measurement mode. The firmware
	 * updates the measurement values depending on the measurement mode.
	 *
	 * @param co2 CO₂ concentration in ppm
	 *
	 * @param temperature Temperature in °C
	 *
	 * @param humidity Relative humidity in %RH
	 *
	 * @retval 0 success
	 * @retval 1 i2c data too long to fit in transmit buffer
	 * @retval 2 i2c received NACK on transmit of address
	 * @retval 3 i2c received NACK on transmit of data
	 * @retval 4 i2c other error
	 * @retval 5 i2c timeout
	 * @retval 6 bytesReceived(%i) != bytesRequested(%i)
	 * @retval 7 measurement out of range
	 */
	uint8_t readMeasurement(double& co2, double& temperature,
							double& humidity);

	/**
	 * getDataReadyFlag() - Check whether new measurement data is available
	 * for read-out.
	 *
	 * @param dataReadyFlag True if valid data is available, false otherwise.
	 *
	 * @return 0 on success, an i2c error code otherwise
	 */
	bool isDataReady();

	/**
	 * setSelfCalibrationMode() - a blocking call to set the calibration mode and store it in the EEPROM of the SCD4x
	 *
	 * @note The automatic self calibration algorithm assumes that the sensor is exposed to the atmospheric CO2
	 * concentration of 400 ppm at least once per week.
	 *
	 * @note To avoid unnecessary wear of the EEPROM, the setSelfCalibrationMode command should only be used sparingly.
	 *
	 * @param turn on or off self calibration
	 *
	 * @retval 0 success
	 * @retval 1 i2c data too long to fit in transmit buffer
	 * @retval 2 i2c received NACK on transmit of address
	 * @retval 3 i2c received NACK on transmit of data
	 * @retval 4 i2c other error
	 * @retval 5 i2c timeout
	 */
	uint8_t setCalibrationMode(bool enableAutoCalibration);

	/**
	 * getCalibrationMode()
	 *
	 * @return is auto calibration enabled
	 */
	bool getCalibrationMode();

	/**
	 * saveSettings() - store settings in the EEPROM of the SCD4x, wait atleast 800ms before sending further commands
	 * @note To avoid unnecessary wear of the EEPROM, the saveSettings command should only be used sparingly.
	 * EEPROM is guaranteed to endure at least 2000 write cycles before failure.
	 *
	 * @retval 0 success
	 * @retval 1 i2c data too long to fit in transmit buffer
	 * @retval 2 i2c received NACK on transmit of address
	 * @retval 3 i2c received NACK on transmit of data
	 * @retval 4 i2c other error
	 * @retval 5 i2c timeout
	 */
	uint8_t saveSettings();

   private:
	uint8_t _error = 0;
	uint8_t _isValid = false;
	int _address = SCD4X_I2C_ADDRESS;
	bool _init = false;
	TwoWire* _i2cPort = &Wire;
	Stream* _debug_output_stream = &Serial;

	bool inRange(double value, double max, double min) {
		return !(value <= min) && (value <= max);
	}

	void _commandSequence(uint16_t registerAddress) {
		_i2cPort->beginTransmission(_address);
		_i2cPort->write(highByte(registerAddress));
		_i2cPort->write(lowByte(registerAddress));
		_error = _i2cPort->endTransmission(true);  // send stop bit
	}

	uint16_t _readSequence(uint16_t registerAddress) {
		const int bytesRequested = 3;  // check bit at end

		_i2cPort->beginTransmission(_address);
		_i2cPort->write(highByte(registerAddress));
		_i2cPort->write(lowByte(registerAddress));
		_error = _i2cPort->endTransmission(false);	// no stop bit

		if (_error == 0) {
			uint8_t bytesReceived = _i2cPort->requestFrom(_address, bytesRequested);
			if (bytesReceived == bytesRequested) {	// If received requested amount of bytes
				uint8_t data[bytesReceived];
				_i2cPort->readBytes(data, bytesReceived);
				return ((uint16_t)data[0] << 8 | data[1]);

			} else {
				ESP_LOGE("Wire.requestFrom",
						 "bytesReceived(%i) != bytesRequested(%i)", bytesReceived,
						 bytesRequested);
				return 0;
			}
		} else {
			ESP_LOGE("Wire.endTransmission", "Returned ERROR");
			return 0;
		}
	}

	void _writeSequence(uint16_t registerAddress, uint16_t value, uint8_t checkSum) {
		_i2cPort->beginTransmission(_address);
		_i2cPort->write(highByte(registerAddress));
		_i2cPort->write(lowByte(registerAddress));

		_i2cPort->write(highByte(value));
		_i2cPort->write(lowByte(value));
		_i2cPort->write(checkSum);				   // Checksum

		_error = _i2cPort->endTransmission(true);  // stop bit
	}

};
