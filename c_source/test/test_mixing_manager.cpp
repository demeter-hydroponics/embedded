#include <gtest/gtest.h>

#include "MixingManager.hpp"
#include "MockMessageQueue.hpp"
#include "MockMixingDevice.hpp"
#include "MockTime.hpp"

using namespace ::testing;

TEST(MixingManagerTest, TestInitToMixing) {
    MockMixingDevice mixingDevice;
    MockMessageQueue<SetMixingStateCommand> mixingStateCommandQueue;
    MockTimeServer timeServer;
    MixingManager mixingManager(mixingDevice, mixingStateCommandQueue, timeServer);

    EXPECT_EQ(mixingManager.getState(), MixingManager::State::INIT);

    mixingManager.run();

    EXPECT_EQ(mixingManager.getState(), MixingManager::State::MIXING);

    testing::InSequence seq;
}

TEST(MixingManagerTest, TestMixingToOverride) {
    MockMixingDevice mixingDevice;
    MockMessageQueue<SetMixingStateCommand> mixingStateCommandQueue;
    MockTimeServer timeServer;
    MixingManager mixingManager(mixingDevice, mixingStateCommandQueue, timeServer);

    InSequence seq;

    EXPECT_EQ(mixingManager.getState(), MixingManager::State::INIT);
    mixingManager.run();
    EXPECT_EQ(mixingManager.getState(), MixingManager::State::MIXING);
    // clear expectaions about the mixing device
    Mock::VerifyAndClearExpectations(&mixingDevice);
    Mock::VerifyAndClearExpectations(&mixingStateCommandQueue);

    SetMixingStateCommand command;
    command.State = MixingOverrideState_OVERRIDE_VALVE_ON;

    EXPECT_CALL(mixingStateCommandQueue, receive(testing::_)).WillOnce(DoAll(SetArgReferee<0>(command), Return(true)));
    mixingManager.run();

    Mock::VerifyAndClearExpectations(&mixingDevice);
    Mock::VerifyAndClearExpectations(&mixingStateCommandQueue);

    EXPECT_EQ(mixingManager.getState(), MixingManager::State::OVERRIDE);
    EXPECT_CALL(mixingDevice, controlNutrientMixingValve(true)).Times(1);
    mixingManager.run();

    Mock::VerifyAndClearExpectations(&mixingDevice);
    Mock::VerifyAndClearExpectations(&mixingStateCommandQueue);

    command.State = MixingOverrideState_OVERRIDE_VALVE_OFF;
    EXPECT_CALL(mixingStateCommandQueue, receive(testing::_)).WillOnce(DoAll(SetArgReferee<0>(command), Return(true)));
    EXPECT_EQ(mixingManager.getState(), MixingManager::State::OVERRIDE);
    EXPECT_CALL(mixingDevice, controlNutrientMixingValve(false)).Times(1);
    mixingManager.run();
}