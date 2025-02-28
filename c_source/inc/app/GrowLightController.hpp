#ifndef GROW_LIGHT_CONTROLLER_HPP
#define GROW_LIGHT_CONTROLLER_HPP

#include "GrowLightSection.hpp"
#include "time.hpp"
#include "MessageQueue.hpp"
#include "node/commands.pb.h"
#include "CommManagerTypes.hpp"

class GrowLightController {
   public:
    GrowLightController(TimeServer& timeServer, BaseGrowLightSection* growLightSections, size_t numGrowLightSections, MessageQueue<CommManagerQueueData_t>& msgQueue, MessageQueue<SetPPFDReferenceCommand>& ppfdCommandQueue);

    void run();

    /**
     * @brief Set the PPFD reference for the grow light controller
     * @param ppfdReference The PPFD reference to set
     * @return void
     */
    void setPPFDReference(float ppfdReference);

   private:
    TimeServer& timeServer_;
    BaseGrowLightSection* growLightSections_;
    size_t numGrowLightSections_;
    MessageQueue<CommManagerQueueData_t>& msgQueue_;
    MessageQueue<SetPPFDReferenceCommand>& ppfdCommandQueue_;
    constexpr static float MAX_PPFD_OUTPUT = 300.0F;

    float ppfdReference_ = 0.0F;
    constexpr static float K_INTEGRAL_PPFD_TO_PPFD = 0.001F * 277.77F;

    constexpr static size_t MAX_NUM_GROWLIGHT_SECTIONS = 5U;
    float integral_error[MAX_NUM_GROWLIGHT_SECTIONS] = {0};
};

#endif // GROW_LIGHT_CONTROLLER_HPP