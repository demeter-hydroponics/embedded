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

#ifndef LTR303_h
#define LTR303_h

#include "ESPHAL_I2C.hpp"
#include "LightSensor.hpp"

#define LTR303_LIB_VERSION "1.1.0"

// Default I2C address for the sensor
#define LTR303_ADDR 0x29

// Register addresses for the sensor
enum {
    LTR303_CONTR = 0x80,
    LTR303_MEAS_RATE = 0x85,
    LTR303_PART_ID = 0xA0,
    LTR303_MANUFACTURER_ID = 0x05,
    LTR303_DATA_CH1_0 = 0x88,
    LTR303_DATA_CH1_1 = 0x89,
    LTR303_DATA_CH0_0 = 0x8A,
    LTR303_DATA_CH0_1 = 0x8B,
    LTR303_STATUS = 0x8C
};

#define AUTO_GAIN_UNDEREXPOSED_THRESHOLD 655   // Determines if either sensor underexposed (655 = ~1% of max raw)
#define AUTO_GAIN_OVEREXPOSED_THRESHOLD 58982  // Determines if either sensor overexposed (58982 = ~50 % of max raw)

// Enum for gain settings (1X to 96X)
enum ltr303Gain { GAIN_1X = 0, GAIN_2X = 1, GAIN_4X = 2, GAIN_8X = 3, GAIN_48X = 6, GAIN_96X = 7 };

// Enum for exposure settings (50ms to 400ms)
enum ltr303Exposure {
    EXPOSURE_50ms = 1,
    EXPOSURE_100ms = 0,
    EXPOSURE_150ms = 4,
    EXPOSURE_200ms = 2,
    EXPOSURE_250ms = 5,
    EXPOSURE_300ms = 6,
    EXPOSURE_350ms = 7,
    EXPOSURE_400ms = 3
};

// Enum for measurement intervals (50ms, 100ms, etc.)
enum ltr303Interval { LTR303_50ms_INTERVAL = 0, LTR303_100ms_INTERVAL = 1, LTR303_200ms_INTERVAL = 2, LTR303_500ms_INTERVAL = 3 };

class LTR303 : public BaseLightSensor {
   public:
    LTR303(ESPHAL_I2C &i2c_dev) : i2c_dev_(i2c_dev) {}

    /**
     * Initializes the LTR303 sensor with specified gain, exposure, and auto-gain settings.
     *
     * This function sets up the LTR303 sensor for measurement. It takes in parameters for gain, exposure time, and auto-gain
     * settings.
     * @param gain The gain setting for the sensor (GAIN_1X, GAIN_2X, etc.)
     * @param exposure The exposure time for each ALs cycle
     * @param enableAutoGain Whether to adjust the gain when new data is received
     * @param port The I2C port to use (default: Wire)
     * @return 0 if successful, or an error code otherwise
     */
    uint8_t begin(ltr303Gain gain, ltr303Exposure exposure, bool enableAutoGain, uint8_t addr = LTR303_ADDR);

    /**
     * Starts periodic measurement mode on the LTR303 sensor.
     *
     * This function starts the LTR303 sensor in periodic measurement mode. It will continue to measure data until
     * endPeriodicMeasurement is called.
     * @return 0 if successful, or an error code otherwise
     */
    uint8_t startPeriodicMeasurement();

    /**
     * Stops periodic measurement mode on the LTR303 sensor.
     *
     * This function stops the LTR303 sensor from measuring data and returns it to idle mode.
     * @return 0 if successful, or an error code otherwise
     */
    uint8_t endPeriodicMeasurement();

    /**
     * Checks whether the device is correctly connected and returns a string describing any errors.
     *
     * This function checks the connection status of the LTR303 sensor. If there are any issues with the connection, it will
     * return a descriptive error message.
     * @param port The I2C port to use (default: Wire)
     * @param stream The debug output stream (default: &Serial)
     * @param addr The I2C address of the sensor (default: LTR303_ADDR)
     * @return true if device correctly connected, otherwise false
     */
    bool isConnected(uint8_t addr = LTR303_ADDR);

    /**
     * Retrieves raw light count data from the LTR303 sensor without gain or exposure compensation.
     *
     * This function retrieves raw data from the LTR303 sensor. The raw data is not adjusted for gain or exposure settings.
     * @param visibleAndIRraw The variable to store the visible and IR sensor data
     * @param IRraw The variable to store the IR sensor output data
     * @return 0 if successful, or an error code otherwise. If no new data is available, returns 7.
     */
    uint8_t getData(uint16_t &visibleAndIRraw, uint16_t &IRraw);

    /**
     * Retrieves raw light count data from the LTR303 sensor, checks validity, adjusts for gain and exposure, and scales to an
     * approximate lux value.
     *
     * This function retrieves raw data from the LTR303 sensor, validates it, and then scales it to an approximate lux value. The
     * exact calculation used is dependent on the gain and exposure settings specified in begin.
     * @param lux The variable to store the output value
     * @return true if the data returned is valid, otherwise false
     */
    bool getApproximateLux(double &lux);

    /**
     * Checks if new data is available for retrieval
     *
     * @return true if data is available, otherwise false
     */
    bool newDataAvailable();

    /**
     * Converts an error code into descriptive text.
     *
     * This function takes an error code and returns a string describing what that error means. It can be used to display error
     * messages to users.
     * @param errorCode The error returned by function that you want to convert
     * @return A pointer to a const char array containing the descriptive text of the error. If the error code is not recognized,
     * "Unknown error" is returned.
     * @note The char array will never be longer than 128 characters
     */
    const char *getErrorText(uint8_t errorCode);

    ErrorCode getLightLux(float &lux) override;

   private:
    // Resets the sensor to its default state
    uint8_t reset();

    // Sets the gain, reset register, and mode of the LTR303
    uint8_t setControlRegister(bool reset = false, bool mode = true);

    // Adjusts the gain based on sensor readings (if auto-gain is enabled)
    bool autoGain(uint16_t visibleAndIRraw);

    // Sets the exposure time for each ALs cycle and measurement interval
    uint8_t setExposureTime();

    // Reads a 16-bit integer from a specified address
    uint8_t read16bitInt(uint8_t address, uint16_t &value);

    // The I2C address of the LTR303 sensor.
    uint8_t _i2c_address = LTR303_ADDR;

    // The I2C port used to communicate with the LTR303 sensor.
    ESPHAL_I2C &i2c_dev_;

    // An error code returned by various functions in the class.
    uint8_t _error = 0;

    // A flag indicating whether new data is available from the LTR303 sensor.
    bool _dataValid = false;

    // The current gain setting for the LTR303 sensor.
    ltr303Gain _gain = GAIN_1X;

    // The compensation factor for the gain of the LTR303 sensor.
    double _gainCompensation = 1;  // set for 1x

    // The current exposure time set for the LTR303 sensor.
    ltr303Exposure _exposure = EXPOSURE_100ms;

    // The compensation factor for the exposure time of the LTR303 sensor.
    double _exposureCompensation = 0.1;  // set for 100ms

    // A flag indicating whether automatic gain adjustment is enabled for the LTR303 sensor.
    bool _autoGainEnabled = true;
};
#endif