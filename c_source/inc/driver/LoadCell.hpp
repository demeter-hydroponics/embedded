#ifndef LOAD_CELL_HPP
#define LOAD_CELL_HPP

#include <cstdint>

#include "HAL_GPIO.hpp"
#include "time.hpp"

class BaseLoadCell {
   public:
    enum class ErrorCode {
        NO_ERROR,
        INIT_ERROR,
        COMM_ERROR,
    };

    virtual ErrorCode getDifferentialVoltageV(float& voltage) = 0;
};

class HX711_BitBang : public BaseLoadCell {
   public:
    enum class OperationalMode {
        CH_A_128 = 1,
        CH_B_32 = 2,
        CH_A_64 = 3,
    };

    /// @brief Full scale range in volts for each operational mode
    constexpr inline static float full_scale_range_from_op_mode[] = {0.020F, 0.040F, 0.080F};

    constexpr float get_full_scale_range(OperationalMode opmode) noexcept {
        auto mode_num = static_cast<uint8_t>(opmode);
        return (mode_num < 3) ? HX711_BitBang::full_scale_range_from_op_mode[mode_num] : 0.0F;
    }

    ErrorCode init();

    HX711_BitBang(HAL_GPIO& dataPin, HAL_GPIO& clockPin, TimeServer& timeServer, OperationalMode opmode);
    ErrorCode getDifferentialVoltageV(float& voltage) override;

   private:
    HAL_GPIO& dataPin_;
    HAL_GPIO& clockPin_;
    TimeServer& timeServer_;
    OperationalMode opmode_;
};

#endif  // LOAD_CELL_HPP