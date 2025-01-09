#ifndef ESPHAL_GPIO_HPP
#define ESPHAL_GPIO_HPP

#include "HAL_GPIO.hpp"
#include "driver/gpio.h"

class ESPHAL_GPIO : public HAL_GPIO {
   public:
    ESPHAL_GPIO(gpio_num_t pin);
    ErrorCode setPinMode(PinMode mode) override;
    ErrorCode writePin(bool value) override;
    ErrorCode readPin(bool& value) override;

   private:
    gpio_num_t pin_;
    PinMode mode_ = PinMode::HIGH_Z;
};

#endif  // ESPHAL_GPIO_HPP