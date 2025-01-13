#ifndef ESPHAL_VEML_7700_HPP
#define ESPHAL_VEML_7700_HPP

#include "LightSensor.hpp"
#include "esp_veml_7700.h"

class VEML7700 : public BaseLightSensor {
   public:
    VEML7700();

    bool init(i2c_master_bus_handle_t i2c_master_handle);

    ErrorCode getLightLux(float& light) override;

   private:
    veml7700_handle_t dev_;

    bool initialized_ = false;
};

#endif  // ESPHAL_VEML_7700_HPP