#include "GrowLightController.hpp"

#include "time.hpp"

GrowLightController::GrowLightController(TimeServer& timeServer, BaseGrowLightSection** growLightSections,
                                         size_t numGrowLightSections, MessageQueue<CommManagerQueueData_t>& msgQueue,
                                         MessageQueue<SetPPFDReferenceCommand>& ppfdCommandQueue)
    : timeServer_(timeServer),
      growLightSections_(growLightSections),
      numGrowLightSections_(numGrowLightSections),
      msgQueue_(msgQueue),
      ppfdCommandQueue_(ppfdCommandQueue) {}

void GrowLightController::setPPFDReference(float ppfdReference) { ppfdReference_ = ppfdReference; }

void GrowLightController::run() {
    if (timeServer_.getUtimeUs() > (lastRunTime_ + GROW_LIGHT_CONTROLLER_RUN_PERIOD_US)) {
        lastRunTime_ = timeServer_.getUtimeUs();
    } else {
        return;
    }

    // Check for new PPFD reference commands
    SetPPFDReferenceCommand ppfdCommand;
    if (ppfdCommandQueue_.receive(ppfdCommand)) {
        setPPFDReference(ppfdCommand.PPFD);
    }

    for (size_t i = 0; i < numGrowLightSections_; i++) {
        BaseGrowLightSection* growLightSection = growLightSections_[i];
        float sensed_ppfd = 0.0F;
        if (growLightSection->getSensedPPFD(sensed_ppfd) == BaseGrowLightSection::ErrorCode::NO_ERROR) {
            const float ppfd_error = ppfdReference_ - sensed_ppfd;
            integral_error[i] += ppfd_error;
            growLightSection->setOutputPPFD(integral_error[i] * K_INTEGRAL_PPFD_TO_PPFD);
        }
    }
}