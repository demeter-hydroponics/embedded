#include "MixingDevice.hpp"

#include "MessageQueue.hpp"
#include "TDSSense.hpp"
#include "messages/pump/mixing_stats.pb.h"
#include "pHSense.hpp"
#include "util.hpp"

MixingDevice::MixingDevice(BasepHSense& pHSense, BaseTDSSense* TDSSense, MessageQueue<MixingTankStats>& messageQueue)
    : pHSense_(pHSense),
      TDSSense_(TDSSense),
      messageQueue_(messageQueue),
      pH_error_(BasepHSense::ErrorCode::ADC_ERROR),
      TDS_error_(TDSSense::ErrorCode::ADC_ERROR) {}

MixingDevice::ErrorCode MixingDevice::run() {
    MixingDevice::ErrorCode err = MixingDevice::ErrorCode::NO_ERROR;

    float pH = 0.0F;
    pH_error_ = pHSense_.get_pH(pH);
    if (pH_error_ != BasepHSense::ErrorCode::NO_ERROR) {
        err = MixingDevice::ErrorCode::PH_READ_ERROR;
    } else {
        pH_ = pH;
    }

    float rawVoltagepH = 0.0F;
    IGNORE(pHSense_.get_rawVoltage(rawVoltagepH));

    const bool TDS_enabled = TDSSense_ != nullptr;
    float TDS = 000.0F;
    float rawVoltageTDS = 0.0F;
    if (TDS_enabled) {
        TDS_error_ = TDSSense_->get_TDS_concentration(TDS);

        if (TDS_error_ == TDSSense::ErrorCode::NO_ERROR) {
            TDS_ = TDS;
        }

        IGNORE(TDSSense_->get_rawVoltage(rawVoltageTDS));
    }

    MixingTankStats mixingTankStats;
    mixingTankStats.TDSSense.TDSSensePPM = TDS;
    mixingTankStats.TDSSense.analogVoltage = rawVoltageTDS;
    mixingTankStats.TDSSense.Validity =
        (TDS_error_ == TDSSense::ErrorCode::NO_ERROR) ? SensorValidity_VALID : SensorValidity_INVALID;
    mixingTankStats.pHSense.phSenseMolPerL = pH;
    mixingTankStats.pHSense.analogVoltage = rawVoltagepH;
    mixingTankStats.pHSense.Validity =
        (pH_error_ == BasepHSense::ErrorCode::NO_ERROR) ? SensorValidity_VALID : SensorValidity_INVALID;

    // Send the message to the message queue
    IGNORE(messageQueue_.send(mixingTankStats));

    return err;
}

MixingDevice::ErrorCode MixingDevice::get_pH(float& pH) {
    pH = pH_;
    return pH_error_ == BasepHSense::ErrorCode::NO_ERROR ? MixingDevice::ErrorCode::NO_ERROR
                                                         : MixingDevice::ErrorCode::PH_READ_ERROR;
}

MixingDevice::ErrorCode MixingDevice::get_TDS(float& TDS) {
    TDS = TDS_;
    return TDS_error_ == TDSSense::ErrorCode::NO_ERROR ? MixingDevice::ErrorCode::NO_ERROR
                                                       : MixingDevice::ErrorCode::TDS_READ_ERROR;
}
