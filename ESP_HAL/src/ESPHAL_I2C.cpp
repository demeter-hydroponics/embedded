#include "ESPHAL_I2C.hpp"

ESPHAL_I2C::ESPHAL_I2C(i2c_master_bus_config_t const &bus_config, const uint32_t freq) : freq_(freq), bus_config_(bus_config) {}

ESPHAL_I2C::ErrorCode ESPHAL_I2C::init() {
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config_, &bus_handle_));
    return ErrorCode::NO_ERROR;
}

ESPHAL_I2C::ErrorCode ESPHAL_I2C::write(uint8_t address, uint8_t const *const data, size_t length) {
    ErrorCode ret = ErrorCode::NO_ERROR;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = freq_,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle_, &dev_cfg, &dev_handle));

    if (i2c_master_transmit(dev_handle, data, length, 1000) != ESP_OK) {
        ret = ErrorCode::ERROR_NO_ACK_RECEIVED;
    }

    ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));

    return ret;
}

ESPHAL_I2C::ErrorCode ESPHAL_I2C::read(uint8_t address, uint8_t *const data, size_t length) {
    ErrorCode ret = ErrorCode::NO_ERROR;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = freq_,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle_, &dev_cfg, &dev_handle));

    if (i2c_master_receive(dev_handle, data, length, 1000) != ESP_OK) {
        ret = ErrorCode::ERROR_NO_ACK_RECEIVED;
    }

    ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));

    return ret;
}