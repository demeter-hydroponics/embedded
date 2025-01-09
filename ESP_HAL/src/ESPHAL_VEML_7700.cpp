#include "ESPHAL_VEML_7700.hpp"

VEML7700::VEML7700() {}

bool VEML7700::init(int i2c_master_num) {
    esp_err_t init_result = veml7700_initialize(&dev_, i2c_master_num);

    initialized_ = init_result == ESP_OK;

    return initialized_;
}

VEML7700::ErrorCode VEML7700::getLightLux(float& light) {
    ErrorCode error = initialized_ ? ErrorCode::NO_ERROR : ErrorCode::NOT_INITIALIZED;

    if (error == ErrorCode::NO_ERROR) {
        double readLux;
        esp_err_t read_result = veml7700_read_als_lux_auto(dev_, &readLux);

        light = readLux;

        error = read_result == ESP_OK ? ErrorCode::NO_ERROR : ErrorCode::COMM_ERROR;
    }

    return error;
}