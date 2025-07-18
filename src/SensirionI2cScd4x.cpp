/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 *
 * Generator:     sensirion-driver-generator 1.1.2
 * Product:       scd4x
 * Model-Version: 2.0
 */
/*
 * Copyright (c) 2025, Sensirion AG
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

#include "SensirionI2cScd4x.h"
#include <Arduino.h>

// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

#define ROUND(x) ((int32_t)((x) + 0.5))

static uint8_t communication_buffer[9] = {0};

SensirionI2cScd4x::SensirionI2cScd4x() {
}

float SensirionI2cScd4x::signalTemperature(uint16_t rawTemperature) {
    float temperature = 0.0;
    temperature = -45.0 + ((175.0 * rawTemperature) / 65535.0);
    return temperature;
}

float SensirionI2cScd4x::signalRelativeHumidity(uint16_t rawRelativeHumidity) {
    float relativeHumidity = 0.0;
    relativeHumidity = (100.0 * rawRelativeHumidity) / 65535.0;
    return relativeHumidity;
}

uint16_t
SensirionI2cScd4x::signalCo2Concentration(uint16_t rawCo2Concentration) {
    uint16_t co2Concentration = 0;
    co2Concentration = rawCo2Concentration;
    return co2Concentration;
}

float SensirionI2cScd4x::signalTemperatureOffset(
    uint16_t rawTemperatureOffset) {
    float temperatureOffset = 0.0;
    temperatureOffset = (175 * rawTemperatureOffset) / 65535.0;
    return temperatureOffset;
}

uint32_t SensirionI2cScd4x::signalAmbientPressure(uint16_t rawAmbientPressure) {
    uint32_t ambientPressure = 0;
    ambientPressure = (uint32_t)rawAmbientPressure * 100;
    return ambientPressure;
}

int16_t SensirionI2cScd4x::readMeasurement(uint16_t& aCo2Concentration,
                                           float& aTemperature,
                                           float& aRelativeHumidity) {
    uint16_t rawCo2Concentration = 0;
    uint16_t rawTemperature = 0;
    uint16_t rawRelativeHumidity = 0;
    int16_t localError = 0;
    localError = readMeasurementRaw(rawCo2Concentration, rawTemperature,
                                    rawRelativeHumidity);
    if (localError != NO_ERROR) {
        return localError;
    }
    aCo2Concentration =
        SensirionI2cScd4x::signalCo2Concentration(rawCo2Concentration);
    aTemperature = SensirionI2cScd4x::signalTemperature(rawTemperature);
    aRelativeHumidity =
        SensirionI2cScd4x::signalRelativeHumidity(rawRelativeHumidity);
    return localError;
}

int16_t SensirionI2cScd4x::setTemperatureOffset(float temperatureOffset) {
    int16_t localError = 0;
    uint16_t rawTemperatureOffset =
        (uint16_t)ROUND((temperatureOffset * 65535.0) / 175.0);
    localError = setTemperatureOffsetRaw(rawTemperatureOffset);
    if (localError != NO_ERROR) {
        return localError;
    }
    return localError;
}

int16_t SensirionI2cScd4x::getTemperatureOffset(float& aTemperatureOffset) {
    uint16_t rawTemperatureOffset = 0;
    int16_t localError = 0;
    localError = getTemperatureOffsetRaw(rawTemperatureOffset);
    if (localError != NO_ERROR) {
        return localError;
    }
    aTemperatureOffset =
        SensirionI2cScd4x::signalTemperatureOffset(rawTemperatureOffset);

    return localError;
}

int16_t SensirionI2cScd4x::setAmbientPressure(uint32_t ambientPressure) {
    int16_t localError = 0;
    uint16_t rawAmbientPressure = (uint16_t)ROUND(ambientPressure / 100.0);
    localError = setAmbientPressureRaw(rawAmbientPressure);
    if (localError != NO_ERROR) {
        return localError;
    }
    return localError;
}

int16_t SensirionI2cScd4x::getAmbientPressure(uint32_t& aAmbientPressure) {
    uint16_t rawAmbientPressure = 0;
    int16_t localError = 0;
    localError = getAmbientPressureRaw(rawAmbientPressure);
    if (localError != NO_ERROR) {
        return localError;
    }
    aAmbientPressure =
        SensirionI2cScd4x::signalAmbientPressure(rawAmbientPressure);

    return localError;
}

int16_t SensirionI2cScd4x::getDataReadyStatus(bool& arg0) {
    uint16_t dataReadyStatus = 0;
    int16_t localError = 0;
    localError = getDataReadyStatusRaw(dataReadyStatus);
    if (localError != NO_ERROR) {
        return localError;
    }
    arg0 = (dataReadyStatus & 2047) != 0;
    ;
    return localError;
}

int16_t
SensirionI2cScd4x::getSensorVariant(SCD4xSensorVariant& aSensorVariant) {
    SCD4xSensorVariant retVal = SCD4X_SENSOR_VARIANT_MASK;
    uint16_t rawSensorVariant = 0;
    uint16_t mySensorVariant = 0;
    int16_t localError = 0;
    retVal = SCD4X_SENSOR_VARIANT_MASK;
    uint16_t mask = (uint16_t)(retVal);
    localError = getSensorVariantRaw(rawSensorVariant);
    if (localError != NO_ERROR) {
        return localError;
    }
    mySensorVariant = (uint16_t)(rawSensorVariant & mask);
    if (mySensorVariant == (uint16_t)(SCD4X_SENSOR_VARIANT_SCD40)) {
        retVal = SCD4X_SENSOR_VARIANT_SCD40;
    } else if (mySensorVariant == (uint16_t)(SCD4X_SENSOR_VARIANT_SCD41)) {
        retVal = SCD4X_SENSOR_VARIANT_SCD41;
    } else if (mySensorVariant == (uint16_t)(SCD4X_SENSOR_VARIANT_SCD42)) {
        retVal = SCD4X_SENSOR_VARIANT_SCD42;
    } else if (mySensorVariant == (uint16_t)(SCD4X_SENSOR_VARIANT_SCD43)) {
        retVal = SCD4X_SENSOR_VARIANT_SCD43;
    }
    aSensorVariant = retVal;
    return localError;
}

int16_t SensirionI2cScd4x::measureAndReadSingleShot(uint16_t& aCo2Concentration,
                                                    float& aTemperature,
                                                    float& aRelativeHumidity) {
    bool dataReady = false;
    int16_t localError = 0;
    localError = measureSingleShot();
    if (localError != NO_ERROR) {
        return localError;
    }
    localError = getDataReadyStatus(dataReady);
    if (localError != NO_ERROR) {
        return localError;
    }
    while (!dataReady) {
        delay(100);
        localError = getDataReadyStatus(dataReady);
        if (localError != NO_ERROR) {
            return localError;
        }
    }
    localError =
        readMeasurement(aCo2Concentration, aTemperature, aRelativeHumidity);
    return localError;
}

int16_t SensirionI2cScd4x::startPeriodicMeasurement() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x21b1, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    return localError;
}

int16_t SensirionI2cScd4x::readMeasurementRaw(uint16_t& co2Concentration,
                                              uint16_t& temperature,
                                              uint16_t& relativeHumidity) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xec05, buffer_ptr, 9);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 9);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 9,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(co2Concentration);
    localError |= rxFrame.getUInt16(temperature);
    localError |= rxFrame.getUInt16(relativeHumidity);
    return localError;
}

int16_t SensirionI2cScd4x::stopPeriodicMeasurement() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3f86, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(500);
    return localError;
}

int16_t SensirionI2cScd4x::setTemperatureOffsetRaw(uint16_t offsetTemperature) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x241d, buffer_ptr, 5);
    localError |= txFrame.addUInt16(offsetTemperature);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t
SensirionI2cScd4x::getTemperatureOffsetRaw(uint16_t& offsetTemperature) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2318, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(offsetTemperature);
    return localError;
}

int16_t SensirionI2cScd4x::setSensorAltitude(uint16_t sensorAltitude) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2427, buffer_ptr, 5);
    localError |= txFrame.addUInt16(sensorAltitude);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cScd4x::getSensorAltitude(uint16_t& sensorAltitude) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2322, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(sensorAltitude);
    return localError;
}

int16_t SensirionI2cScd4x::setAmbientPressureRaw(uint16_t ambientPressure) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xe000, buffer_ptr, 5);
    localError |= txFrame.addUInt16(ambientPressure);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cScd4x::getAmbientPressureRaw(uint16_t& ambientPressure) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xe000, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(ambientPressure);
    return localError;
}

int16_t
SensirionI2cScd4x::performForcedRecalibration(uint16_t targetCO2Concentration,
                                              uint16_t& frcCorrection) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x362f, buffer_ptr, 5);
    localError |= txFrame.addUInt16(targetCO2Concentration);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(400);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 5);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(frcCorrection);
    return localError;
}

int16_t
SensirionI2cScd4x::setAutomaticSelfCalibrationEnabled(uint16_t ascEnabled) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2416, buffer_ptr, 5);
    localError |= txFrame.addUInt16(ascEnabled);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t
SensirionI2cScd4x::getAutomaticSelfCalibrationEnabled(uint16_t& ascEnabled) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2313, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(ascEnabled);
    return localError;
}

int16_t
SensirionI2cScd4x::setAutomaticSelfCalibrationTarget(uint16_t ascTarget) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x243a, buffer_ptr, 5);
    localError |= txFrame.addUInt16(ascTarget);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t
SensirionI2cScd4x::getAutomaticSelfCalibrationTarget(uint16_t& ascTarget) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x233f, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(ascTarget);
    return localError;
}

int16_t SensirionI2cScd4x::startLowPowerPeriodicMeasurement() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x21ac, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    return localError;
}

int16_t SensirionI2cScd4x::getDataReadyStatusRaw(uint16_t& dataReadyStatus) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0xe4b8, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(dataReadyStatus);
    return localError;
}

int16_t SensirionI2cScd4x::persistSettings() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3615, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(800);
    return localError;
}

int16_t SensirionI2cScd4x::getSerialNumber(uint64_t& serialNumber) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3682, buffer_ptr, 9);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 9);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 9,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getInteger(reinterpret_cast<uint8_t*>(&serialNumber),
                                     LongInteger, 6);
    return localError;
}

int16_t SensirionI2cScd4x::performSelfTest(uint16_t& sensorStatus) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3639, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(10000);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(sensorStatus);
    return localError;
}

int16_t SensirionI2cScd4x::performFactoryReset() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3632, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1200);
    return localError;
}

int16_t SensirionI2cScd4x::reinit() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x3646, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(30);
    return localError;
}

int16_t SensirionI2cScd4x::getSensorVariantRaw(uint16_t& sensorVariant) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x202f, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(sensorVariant);
    return localError;
}

int16_t SensirionI2cScd4x::measureSingleShot() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x219d, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(5000);
    return localError;
}

int16_t SensirionI2cScd4x::measureSingleShotRhtOnly() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2196, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(50);
    return localError;
}

int16_t SensirionI2cScd4x::powerDown() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x36e0, buffer_ptr, 2);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cScd4x::wakeUp() {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x36f6, buffer_ptr, 2);
    SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    delay(30);
    return localError;
}

int16_t SensirionI2cScd4x::setAutomaticSelfCalibrationInitialPeriod(
    uint16_t ascInitialPeriod) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2445, buffer_ptr, 5);
    localError |= txFrame.addUInt16(ascInitialPeriod);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cScd4x::getAutomaticSelfCalibrationInitialPeriod(
    uint16_t& ascInitialPeriod) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x2340, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(ascInitialPeriod);
    return localError;
}

int16_t SensirionI2cScd4x::setAutomaticSelfCalibrationStandardPeriod(
    uint16_t ascStandardPeriod) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x244e, buffer_ptr, 5);
    localError |= txFrame.addUInt16(ascStandardPeriod);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    return localError;
}

int16_t SensirionI2cScd4x::getAutomaticSelfCalibrationStandardPeriod(
    uint16_t& ascStandardPeriod) {
    int16_t localError = NO_ERROR;
    uint8_t* buffer_ptr = communication_buffer;
    SensirionI2CTxFrame txFrame =
        SensirionI2CTxFrame::createWithUInt16Command(0x234b, buffer_ptr, 3);
    localError =
        SensirionI2CCommunication::sendFrame(_i2cAddress, txFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    delay(1);
    SensirionI2CRxFrame rxFrame(buffer_ptr, 3);
    localError = SensirionI2CCommunication::receiveFrame(_i2cAddress, 3,
                                                         rxFrame, *_i2cBus);
    if (localError != NO_ERROR) {
        return localError;
    }
    localError |= rxFrame.getUInt16(ascStandardPeriod);
    return localError;
}

void SensirionI2cScd4x::begin(TwoWire& i2cBus, uint8_t i2cAddress) {
    _i2cBus = &i2cBus;
    _i2cAddress = i2cAddress;
}
