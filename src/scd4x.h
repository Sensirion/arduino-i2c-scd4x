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
#ifndef SCD4X_H
#define SCD4X_H

#include <Arduino.h>
#include <Wire.h>

#define SCD4X_I2C_ADDRESS 0x62

class SCD4X {
public:
    /**
     * Initializes this library. Must be called before any other library functions.
     *
     * @param i2cBus Arduino stream object to use for communication
     *
     * @return 0 Success, others on failure
     */
    uint8_t begin(TwoWire& port = Wire, uint8_t addr = SCD4X_I2C_ADDRESS);

    /**
     * Checks for correct response, manufacturer ID, part ID, and does a quick selftest.
     *
     * @param port Wire instance (e.g., Wire or Wire1)
     * @param stream Debug output pointer (e.g., &Serial)
     * @param addr I2C address of the sensor (0x62 by default)
     *
     * @return true if the device is correctly connected, otherwise false
     */
    bool isConnected(TwoWire& port = Wire, Stream* stream = &Serial, uint8_t addr = SCD4X_I2C_ADDRESS);

    /**
     * Starts periodic measurement, with new data available in approximately 5 seconds.
     *
     * @return 0 Success, others on failure
     */
    uint8_t startPeriodicMeasurement();

    /**
     * Stops periodic measurement.
     *
     * @note Wait at least 500ms before sending further commands.
     *
     * @return 0 Success, others on failure
     */
    uint8_t stopPeriodicMeasurement();

    /**
     * Reads sensor output. The measurement data can only be read out once per signal update interval as the buffer is emptied upon read-out.
     *
     * @param co2 CO₂ concentration in ppm
     * @param temperature Temperature in °C
     * @param humidity Relative humidity in %RH
     *
     * @return 0 Success, others on failure
     */
    uint8_t readMeasurement(double& co2, double& temperature, double& humidity);

    /**
     * Checks whether new measurement data is available for read-out.
     *
     * @return true if valid data is available, false otherwise
     */
    bool isDataReady();

    /**
     * Sets the calibration mode and stores it in the EEPROM of the SCD4x.
     *
     * @param enableAutoCalibration Turn on or off self calibration
     *
     * @return 0 Success, others on failure
     */
    uint8_t setCalibrationMode(bool enableAutoCalibration);

    /**
     * Gets the calibration mode.
     *
     * @return true if auto calibration is enabled, false otherwise
     */
    bool getCalibrationMode();

    /**
     * Resets all settings in EEPROM including auto calibration algorithm data
     *
     * @note This is a 12ms blocking function. Be careful not to wear out the EEPROM, which is only guaranteed to endure 2,000 write cycles before failure.
     *
     * @return 0 Success, others on failure
     */
    uint8_t resetEEPROM();

    /**
     * Stores settings in the EEPROM of the SCD4x.
     *
     * @note This is a blocking function. Wait at least 800ms before sending further commands.
     *
     * @return 0 Success, others on failure
     */
    uint8_t saveSettings();

    /**
     * Converts an error code into descriptive text.
     *
     * @param errorCode The error code to convert.
     * @return A pointer to a constant character array containing the descriptive text of the error. If the error code is not recognized, "Unknown error" is returned.
     */
    const char* getErrorText(uint8_t errorCode);

private:
    uint8_t _error = 0;
    bool _settingsChanged = false;
    uint8_t _isValid = false;
    int _address = SCD4X_I2C_ADDRESS;
    TwoWire* _i2cPort = &Wire;
	Stream* debug_output_stream = &Serial;

    bool inRange(double value, double max, double min) {
        return !(value <= min) && (value <= max);
    }

    void _commandSequence(uint16_t registerAddress) {
        _i2cPort->beginTransmission(_address);
        _i2cPort->write(highByte(registerAddress));
        _i2cPort->write(lowByte(registerAddress));
        _error = _i2cPort->endTransmission(true);  // Send stop bit
    }

    uint16_t _readSequence(uint16_t registerAddress) {
        const int bytesRequested = 3;   // Check bit at the end

        _i2cPort->beginTransmission(_address);
        _i2cPort->write(highByte(registerAddress));
        _i2cPort->write(lowByte(registerAddress));
        _error = _i2cPort->endTransmission(false);  // No stop bit

        if (_error == 0) {
            uint8_t bytesReceived = _i2cPort->requestFrom(_address, bytesRequested);
            if (bytesReceived == bytesRequested) {  // If received the requested amount of bytes
                uint8_t data[bytesReceived];
                _i2cPort->readBytes(data, bytesReceived);
                return ((uint16_t)data[0] << 8 | data[1]);
            } else {
                ESP_LOGE("Wire.requestFrom", "bytesReceived(%i) != bytesRequested(%i)", bytesReceived, bytesRequested);
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
        _i2cPort->write(checkSum);

        _error = _i2cPort->endTransmission(true);  // Send stop bit
    }
};

#endif  // SCD4X_H