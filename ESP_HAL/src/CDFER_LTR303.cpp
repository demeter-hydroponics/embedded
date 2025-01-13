/*
        LTR303 sensor library for Arduino
        Lovelesh, thingTronics
        Chris Dirks, keaStudios

Shared under the MIT License

Copyright (c) 2015 thingTronics Limited
Copyright (c) 2023 Chris Dirks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "CDFER_LTR303.hpp"

uint8_t LTR303::begin(ltr303Gain gain, ltr303Exposure exposure, bool enableAutoGain, uint8_t addr) {
    _gain = gain;
    _exposure = exposure;
    _autoGainEnabled = enableAutoGain;
    _i2c_address = addr;
    _error = 0;

    if (isConnected(_i2c_address) == 0) {
        reset();
        setExposureTime();
        startPeriodicMeasurement();
    } else {
        _error = 7;
    }
    return _error;
}

uint8_t LTR303::startPeriodicMeasurement() { return setControlRegister(false, true); }

uint8_t LTR303::endPeriodicMeasurement() { return setControlRegister(false, false); }

bool LTR303::isConnected(uint8_t addr) {
    _i2c_address = addr;

    reset();

#if 0
    _i2cPort->beginTransmission(_i2c_address);
    _error = _i2cPort->endTransmission(true);

    char addrCheck[32];
    if (_error != 0) {
        return false;
    }

    reset();  // not all registers are available when periodic recording is on reset to standby mode
#endif

    /*
        _i2cPort->beginTransmission(_i2c_address);
        _i2cPort->write(LTR303_MANUFACTURER_ID);
        _i2cPort->endTransmission(true);
        uint8_t manufacturerID = 0;
        if (_i2cPort->requestFrom(_i2c_address, (uint8_t)1)) {
            manufacturerID = _i2cPort->read();
        }

        if (manufacturerID != LTR303_MANUFACTURER_ID) {
            return false;
        }

        _i2cPort->beginTransmission(_i2c_address);
        _i2cPort->write(LTR303_PART_ID);
        _i2cPort->endTransmission(true);
        uint8_t partID = 0;
        if (_i2cPort->requestFrom(_i2c_address, (uint8_t)1)) {
            partID = _i2cPort->read();
        }

        if (partID != LTR303_PART_ID) {
            return false;
        }
        */

    bool ret = true;
    const uint8_t reg_addr_data[1] = {LTR303_MANUFACTURER_ID};
    ret &= i2c_dev_.write(_i2c_address, reg_addr_data, sizeof(reg_addr_data)) == ESPHAL_I2C::ErrorCode::NO_ERROR;

    uint8_t manufacturerID[1] = {0};
    ret &= i2c_dev_.read(_i2c_address, manufacturerID, 1) == ESPHAL_I2C::ErrorCode::NO_ERROR;
    ret &= manufacturerID[0] == LTR303_MANUFACTURER_ID;

    if (ret) {
        const uint8_t reg_addr_data[1] = {LTR303_PART_ID};
        ret &= i2c_dev_.write(_i2c_address, reg_addr_data, sizeof(reg_addr_data)) == ESPHAL_I2C::ErrorCode::NO_ERROR;

        uint8_t partID[1] = {0};
        ret &= i2c_dev_.read(_i2c_address, partID, 1) == ESPHAL_I2C::ErrorCode::NO_ERROR;

        ret &= partID[0] == LTR303_PART_ID;
    }

    return ret;
}

uint8_t LTR303::getData(uint16_t &visibleAndIRraw, uint16_t &IRraw) {
    // Gets the 16-bit channel 0 and channel 1 data
    // Default value of both channels is 0x00

    if (newDataAvailable() == true) {
        _error = read16bitInt(LTR303_DATA_CH1_0, IRraw);

        if (_error == 0) {
            return read16bitInt(LTR303_DATA_CH0_0, visibleAndIRraw);
        } else {
            return _error;
        }
    }
    return 7;  // no new data available
}

bool LTR303::getApproximateLux(double &lux) {
    uint16_t visibleAndIRraw = 0, IRraw = 0;

    if (getData(visibleAndIRraw, IRraw) == 0) {
        if (_autoGainEnabled) {
            if (autoGain(visibleAndIRraw) == false) {
                return false;
            }
        }

        // 		( Counts at 1x gain)   counts per second
        lux = (((double)visibleAndIRraw / _gainCompensation) / _exposureCompensation) * 4.86979166667;

        // Serial.printf("%i,%3.2f,%3.2f,%8.2f\n\r", visibleAndIRraw, _gainCompensation, _exposureCompensation, lux);

        return _dataValid;
    }
    return false;
}

bool LTR303::newDataAvailable() {
    // Gets the status information of LTR303
    // Default value is 0x00
    // If valid = false(0), Sensor data is valid (default)
    // If valid = true(1), Sensor data is invalid
    //--------------------------------------------
    // If dataStatus = false(0), OLD data (already read) (default)
    // If dataStatus = true(1), NEW data
    /*
        _i2cPort->beginTransmission(_i2c_address);
        _i2cPort->write(LTR303_STATUS);
        _i2cPort->endTransmission(true);

        if (_i2cPort->requestFrom(_i2c_address, (uint8_t)1) == 1) {
            uint8_t status = _i2cPort->read();

            // Extract validity
            _dataValid = (status & 0x80) ? false : true;

            // Extract data available status
            return (status & 0x04) ? true : false;
        }
        _dataValid = false;
        return false;
        */

    const uint8_t reg_addr_data[1] = {LTR303_STATUS};
    bool writeOkay = i2c_dev_.write(_i2c_address, reg_addr_data, sizeof(reg_addr_data)) == ESPHAL_I2C::ErrorCode::NO_ERROR;
    _error = writeOkay ? 0 : 7;

    if (!writeOkay) {
        return false;
    }

    uint8_t status[1] = {0};
    bool readOkay = i2c_dev_.read(_i2c_address, status, 1) == ESPHAL_I2C::ErrorCode::NO_ERROR;
    _error = readOkay ? 0 : 7;

    if (!readOkay) {
        return false;
    }

    // Extract validity
    _dataValid = (status[0] & 0x80) ? false : true;
    return (status[0] & 0x04) ? true : false;
}

const char *LTR303::getErrorText(uint8_t errorCode) {
    switch (errorCode) {
        case 0:
            return "No Error: Transaction completed successfully";
        case 1:
            return "Data too large to fit in I2C transmission buffer. Try reducing the data size or increasing the buffer size.";
        case 2:
            return "I2C device did not acknowledge address. Check the device wiring and try again.";
        case 3:
            return "I2C device did not acknowledge data. Check the data being transmitted and try again.";
        case 4:
            return "Unknown I2C error occurred. Check the I2C bus for errors or faults.";
        case 5:
            return "I2C transaction timed out. Increase the timeout value or check the I2C wiring.";
        case 6:
            return "Received bytes do not match expected bytes. Check the data transmission and reception code.";
        case 7:
            return "No new data available from sensor. Maybe wait a bit longer or make sure periodic measurements have been "
                   "started";
        default:
            return "Unknown error occurred";
    }
}

// Private functions:

uint8_t LTR303::setControlRegister(bool reset, bool mode) {
    //----------------------------------------
    // If reset = false(0), initial start-up procedure not started (default)
    // If reset = true(1), initial start-up procedure started
    //----------------------------------------
    // If mode = false(0), stand-by mode (default)
    // If mode = true(1), active mode

    switch (_gain) {
        case GAIN_1X:
            _gainCompensation = 1;
            break;
        case GAIN_2X:
            _gainCompensation = 2;
            break;
        case GAIN_4X:
            _gainCompensation = 4;
            break;
        case GAIN_8X:
            _gainCompensation = 8;
            break;
        case GAIN_48X:
            _gainCompensation = 48;
            break;
        case GAIN_96X:
            _gainCompensation = 96;
            break;

        default:
            _gainCompensation = 1;
            break;
    }

    uint8_t controlByte = 0x00;

    // control byte logic
    controlByte |= _gain << 2;
    if (reset) {
        controlByte |= 0x02;
    }

    if (mode) {
        controlByte |= 0x01;
    }

    /*
    _i2cPort->beginTransmission(_i2c_address);
    _i2cPort->write(LTR303_CONTR);
    _i2cPort->write(controlByte);
    return _i2cPort->endTransmission(true);
    */

    const uint8_t reg_addr_data[2] = {LTR303_CONTR, controlByte};
    return i2c_dev_.write(_i2c_address, reg_addr_data, sizeof(reg_addr_data)) == ESPHAL_I2C::ErrorCode::NO_ERROR;
}

bool LTR303::autoGain(uint16_t visibleAndIRraw) {
    if (visibleAndIRraw > AUTO_GAIN_OVEREXPOSED_THRESHOLD) {
        switch (_gain) {
            case GAIN_1X:
                return true;
                break;
            case GAIN_2X:
                _gain = GAIN_1X;
                break;
            case GAIN_4X:
                _gain = GAIN_2X;
                break;
            case GAIN_8X:
                _gain = GAIN_4X;
                break;
            case GAIN_48X:
                _gain = GAIN_8X;
                break;
            case GAIN_96X:
                _gain = GAIN_48X;
                break;

            default:
                break;
        }
        setControlRegister(false, true);
        return false;
    }

    if (visibleAndIRraw < AUTO_GAIN_UNDEREXPOSED_THRESHOLD) {
        switch (_gain) {
            case GAIN_96X:
                return true;
                break;
            case GAIN_1X:
                _gain = GAIN_2X;
                break;
            case GAIN_2X:
                _gain = GAIN_4X;
                break;
            case GAIN_4X:
                _gain = GAIN_8X;
                break;
            case GAIN_8X:
                _gain = GAIN_48X;
                break;
            case GAIN_48X:
                _gain = GAIN_96X;
                break;
            default:
                break;
        }
        setControlRegister(false, true);
        return false;
    }

    return true;
}

uint8_t LTR303::setExposureTime() {
    // Sets the integration time and measurement rate of the sensor
    // integrationTime is the measurement time for each ALs cycle
    // measurementInterval is the interval between DATA_REGISTERS update
    // measurementInterval must be set to be equal or greater than integrationTime
    // Default value is 0x03

    uint8_t measurementByte = 0x00;
    uint8_t measurementInterval;

    switch (_exposure) {
        case EXPOSURE_50ms:
            _exposureCompensation = 0.05;
            measurementInterval = LTR303_50ms_INTERVAL;
            break;

        case EXPOSURE_100ms:
            _exposureCompensation = 0.1;
            measurementInterval = LTR303_100ms_INTERVAL;
            break;

        case EXPOSURE_150ms:
            _exposureCompensation = 0.15;
            measurementInterval = LTR303_200ms_INTERVAL;
            break;

        case EXPOSURE_200ms:
            _exposureCompensation = 0.2;
            measurementInterval = LTR303_200ms_INTERVAL;
            break;

        case EXPOSURE_250ms:
            _exposureCompensation = 0.25;
            measurementInterval = LTR303_500ms_INTERVAL;
            break;

        case EXPOSURE_300ms:
            _exposureCompensation = 0.3;
            measurementInterval = LTR303_500ms_INTERVAL;
            break;

        case EXPOSURE_350ms:
            _exposureCompensation = 0.35;
            measurementInterval = LTR303_500ms_INTERVAL;
            break;

        case EXPOSURE_400ms:
            _exposureCompensation = 0.4;
            measurementInterval = LTR303_500ms_INTERVAL;
            break;

        default:
            _exposureCompensation = 0.1;
            measurementInterval = LTR303_500ms_INTERVAL;
            break;
    }

    measurementByte |= _exposure << 3;
    measurementByte |= measurementInterval;

    const uint8_t reg_addr_data[2] = {LTR303_MEAS_RATE, measurementByte};
    return i2c_dev_.write(_i2c_address, reg_addr_data, sizeof(reg_addr_data)) == ESPHAL_I2C::ErrorCode::NO_ERROR;
}

uint8_t LTR303::reset() { return setControlRegister(true, false); }

uint8_t LTR303::read16bitInt(uint8_t address, uint16_t &value) {
    // Reads an unsigned integer (16 bits) from a LTR303 address (low byte first)
    // Address: LTR303 address (0 to 15), low byte first

    // Check if sensor present for read
    /*
    _i2cPort->beginTransmission(_i2c_address);
    _i2cPort->write(address);
    _error = _i2cPort->endTransmission(true);
    */
    const uint8_t reg_addr_data[1] = {address};
    const bool writeOkay = i2c_dev_.write(_i2c_address, reg_addr_data, sizeof(reg_addr_data)) == ESPHAL_I2C::ErrorCode::NO_ERROR;
    _error = writeOkay ? 0 : 7;

    /*
    if (_error == 0) {
        uint8_t bytesReceived = _i2cPort->requestFrom(_i2c_address, (uint8_t)2);

        if (bytesReceived == 2) {  // If received more than zero bytes
            uint8_t temp[bytesReceived];
            _i2cPort->readBytes(temp, bytesReceived);

            value = temp[1];
            value = (value << 8) | temp[0];

            return 0;  // no error
        }
        return 6;  // no bytes received
    }
    return _error;  // endTransmission Error
    */

    if (!writeOkay) {
        return _error;
    }

    uint8_t temp[2] = {0};
    const bool readOkay = i2c_dev_.read(_i2c_address, temp, 2) == ESPHAL_I2C::ErrorCode::NO_ERROR;
    _error = readOkay ? 0 : 7;

    if (!readOkay) {
        return _error;
    }

    value = temp[1];
    value = (value << 8) | temp[0];

    return 0;
}

BaseLightSensor::ErrorCode LTR303::getLightLux(float &lux) {
    if (newDataAvailable()) {
        double luxDouble;
        bool success = getApproximateLux(luxDouble);
        lux = luxDouble;
        return success ? ErrorCode::NO_ERROR : ErrorCode::COMM_ERROR;
    }
    return ErrorCode::NEW_DATA_NOT_AVAILABLE;
}