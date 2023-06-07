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

#define SCD4X_I2C_ADDRESS 0x62	// I2C Address

class SCD4X {
   public:
	/**
	 * Initializes this library. Must be called before any other library functions.
	 *
	 * @note This function does not start the I2C bus; you must do that before calling this command.
	 *
	 * @param i2cBus Arduino stream object to use for communication.
	 *
	 * @retval 0 Success
	 * @retval 1 I2C data too long to fit in transmit buffer
	 * @retval 2 I2C received NACK on transmit of address
	 * @retval 3 I2C received NACK on transmit of data
	 * @retval 4 I2C other error
	 * @retval 5 I2C timeout
	 */
	uint8_t begin(TwoWire& port = Wire, uint8_t addr = SCD4X_I2C_ADDRESS);

	/**
	 * Checks for correct response, manufacturer ID, part ID, and does a quick selftest.
	 *
	 * @param port Wire instance (e.g., Wire or Wire1)
	 * @param stream Debug output pointer (e.g., &Serial)
	 * @param addr I2C address of the sensor (0x62 by default)
	 *
	 * @returns true if the device is correctly connected, otherwise false
	 */
	bool isConnected(TwoWire& port = Wire, Stream* stream = &Serial, uint8_t addr = SCD4X_I2C_ADDRESS);

	/**
	 * Starts periodic measurement, with new data available in approximately 5 seconds.
	 *
	 * @retval 0 Success
	 * @retval 1 I2C data too long to fit in transmit buffer
	 * @retval 2 I2C received NACK on transmit of address
	 * @retval 3 I2C received NACK on transmit of data
	 * @retval 4 I2C other error
	 * @retval 5 I2C timeout
	 */
	uint8_t startPeriodicMeasurement();

	/**
	 * Stops periodic measurement.
	 *
	 * @note Wait at least 500ms before sending further commands.
	 *
	 * @retval 0 Success
	 * @retval 1 I2C data too long to fit in transmit buffer
	 * @retval 2 I2C received NACK on transmit of address
	 * @retval 3 I2C received NACK on transmit of data
	 * @retval 4 I2C other error
	 * @retval 5 I2C timeout
	 */
	uint8_t stopPeriodicMeasurement();

	/**
	 * Reads sensor output. The measurement data can only be read out once per signal update interval as the buffer is emptied upon read-out.
	 * If no data is available in the buffer, the sensor returns a NACK.
	 * To avoid a NACK response, the isDataReady() function can be used to check the data status.
	 * The I2C master can abort the read transfer with a NACK followed by a STOP condition after any data byte if the user is not interested in subsequent data.
	 *
	 * @note This command is only available in measurement mode. The firmware updates the measurement values depending on the measurement mode.
	 *
	 * @param co2 CO₂ concentration in ppm
	 * @param temperature Temperature in °C
	 * @param humidity Relative humidity in %RH
	 *
	 * @retval 0 Success
	 * @retval 1 I2C data too long to fit in transmit buffer
	 * @retval 2 I2C received NACK on transmit of address
	 * @retval 3 I2C received NACK on transmit of data
	 * @retval 4 I2C other error
	 * @retval 5 I2C timeout
	 * @retval 6 bytesReceived(%i) != bytesRequested(%i)
	 * @retval 7 Measurement out of range
	 */
	uint8_t readMeasurement(double& co2, double& temperature, double& humidity);

	/**
	 * Checks whether new measurement data is available for read-out.
	 *
	 * @param dataReadyFlag true if valid data is available, false otherwise.
	 *
	 * @return true on success, false if there was an I2C error
	 */
	bool isDataReady();

	/**
	 * Sets the calibration mode and stores it in the EEPROM of the SCD4x.
	 *
	 * @note The automatic self-calibration algorithm assumes that the sensor is exposed to the atmospheric CO2 concentration of 400 ppm at least once per week.
	 *
	 * @note To avoid unnecessary wear of the EEPROM, the setSelfCalibrationMode command should only be used sparingly.
	 *
	 * @param enableAutoCalibration Turn on or off self calibration
	 *
	 * @retval 0 Success
	 * @retval 1 I2C data too long to fit in transmit buffer
	 * @retval 2 I2C received NACK on transmit of address
	 * @retval 3 I2C received NACK on transmit of data
	 * @retval 4 I2C other error
	 * @retval 5 I2C timeout
	 */
	uint8_t setCalibrationMode(bool enableAutoCalibration);

	/**
	 * Gets the calibration mode.
	 *
	 * @return true if auto calibration is enabled, false otherwise
	 */
	bool getCalibrationMode();

	/**
	 * Stores settings in the EEPROM of the SCD4x.
	 *
	 * @note Wait at least 800ms before sending further commands.
	 * EEPROM is guaranteed to endure at least 2000 write cycles before failure.
	 *
	 * @retval 0 Success
	 * @retval 1 I2C data too long to fit in transmit buffer
	 * @retval 2 I2C received NACK on transmit of address
	 * @retval 3 I2C received NACK on transmit of data
	 * @retval 4 I2C other error
	 * @retval 5 I2C timeout
	 */
	uint8_t saveSettings();

	/**
	 * @brief Converts an error code into descriptive text.
	 *
	 * This function takes an error code and returns a corresponding descriptive text. It can be used to convert
	 * error codes returned by the SCD4X sensor into human-readable error messages.
	 *
	 * @param errorCode The error code to convert.
	 * @return A pointer to a constant character array containing the descriptive text of the error.
	 *         If the error code is not recognized, "Unknown error" is returned.
	 *
	 * @note The error codes and their meanings are as follows:
	 *   - 0: Success
	 *   - 1: I2C data too long to fit in transmit buffer
	 *   - 2: I2C received NACK on transmit of address
	 *   - 3: I2C received NACK on transmit of data
	 *   - 4: I2C other error
	 *   - 5: I2C timeout
	 *   - 6: bytesReceived(%i) != bytesRequested(%i)
	 *   - 7: Measurement out of range
	 */
	const char* getErrorText(uint8_t errorCode);

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

	/**
	 * Sends a command sequence over the I2C port.
	 *
	 * @param registerAddress The register address to write to.
	 */
	void _commandSequence(uint16_t registerAddress) {
		_i2cPort->beginTransmission(_address);
		_i2cPort->write(highByte(registerAddress));
		_i2cPort->write(lowByte(registerAddress));
		_error = _i2cPort->endTransmission(true);  // Send stop bit
	}

	/**
	 * Reads a sequence of data from the specified register address over the I2C port.
	 *
	 * @param registerAddress The register address to read from.
	 * @return The received data as a 16-bit unsigned integer.
	 */
	uint16_t _readSequence(uint16_t registerAddress) {
		const int bytesRequested = 3;  // Check bit at the end

		_i2cPort->beginTransmission(_address);
		_i2cPort->write(highByte(registerAddress));
		_i2cPort->write(lowByte(registerAddress));
		_error = _i2cPort->endTransmission(false);	// No stop bit

		if (_error == 0) {
			uint8_t bytesReceived = _i2cPort->requestFrom(_address, bytesRequested);
			if (bytesReceived == bytesRequested) {	// If received the requested amount of bytes
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

	/**
	 * Writes a sequence of data to the specified register address over the I2C port.
	 *
	 * @param registerAddress The register address to write to.
	 * @param value The value to write as a 16-bit unsigned integer.
	 * @param checkSum The checksum value to write.
	 */
	void _writeSequence(uint16_t registerAddress, uint16_t value, uint8_t checkSum) {
		_i2cPort->beginTransmission(_address);
		_i2cPort->write(highByte(registerAddress));
		_i2cPort->write(lowByte(registerAddress));

		_i2cPort->write(highByte(value));
		_i2cPort->write(lowByte(value));
		_i2cPort->write(checkSum);	// Checksum

		_error = _i2cPort->endTransmission(true);  // Stop bit
	}
};
