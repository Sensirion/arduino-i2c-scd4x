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

#include <Arduino.h>
#include <Wire.h>

#define SCD4X_I2C_ADDRESS 0x62	// 7-bit I2C Address

class SCD4X {
   public:
	/**
	 * begin() - Initializes the scd4x_Class class.
	 *
	 * @param i2cBus Arduino stream object to use for communication.
	 *
	 */
	uint8_t begin(TwoWire& port = Wire, uint8_t addr = SCD4X_I2C_ADDRESS);

	/**
	 * checks for correct response, manufacturer id and part id.
	 *
	 * @param port Wire instance (e.g Wire or Wire1)
	 * @param stream debug output pointer (e.g. &Serial)
	 * @param addr i2c address of sensor
	 * @returns true if device correctly connected, otherwise false
	 */
	bool isConnected(TwoWire& port = Wire, Stream* stream = &Serial, uint8_t addr = SCD4X_I2C_ADDRESS);

	/**
	 * startPeriodicMeasurement() - start periodic measurement, signal update
	 * interval is 5 seconds.
	 *
	 * @note This command is only available in idle mode.
	 *
	 * @return 0 on success, an error code otherwise
	 */
	uint8_t startPeriodicMeasurement();

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
	 * @return 0 on success, an error code otherwise
	 */
	uint8_t readMeasurement(double& co2, double& temperature,
							double& humidity);

	/**
	 * getDataReadyFlag() - Check whether new measurement data is available
	 * for read-out.
	 *
	 * @param dataReadyFlag True if valid data is available, false otherwise.
	 *
	 * @return 0 on success, an error code otherwise
	 */
	bool isDataReady();

   private:
	uint8_t _error;
	uint8_t _isValid = false;
	int _address;
	bool _init = false;
	TwoWire* _i2cPort;
	Stream* _debug_output_stream = &Serial;

	bool inRange(double value, double max, double min) {
		return !(value <= min) && (value <= max);
	}
};
