#include <gtest/gtest.h>

#include "MixingManager.hpp"
#include "MockMixingDevice.hpp"

using namespace ::testing;

TEST(MixingManagerTest, TestInitToMixing) {
    MockMixingDevice mixingDevice;
    MixingManager mixingManager(mixingDevice);

    EXPECT_EQ(mixingManager.getState(), MixingManager::State::INIT);

    mixingManager.run();

    EXPECT_EQ(mixingManager.getState(), MixingManager::State::MIXING);

    testing::InSequence seq;

    EXPECT_CALL(mixingDevice, get_TDS(testing::_))
        .WillOnce(DoAll(testing::SetArgReferee<0>(0.0f), Return(MixingDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mixingDevice, controlNutrientMixingValve(true)).Times(1);
    mixingManager.run();

    EXPECT_CALL(mixingDevice, get_TDS(testing::_))
        .WillOnce(DoAll(testing::SetArgReferee<0>(1000.0f), Return(MixingDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mixingDevice, controlNutrientMixingValve(false)).Times(1);
    mixingManager.run();

    EXPECT_CALL(mixingDevice, get_TDS(testing::_))
        .WillOnce(DoAll(testing::SetArgReferee<0>(0.0f), Return(MixingDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mixingDevice, controlNutrientMixingValve(true)).Times(1);
    mixingManager.run();

    EXPECT_CALL(mixingDevice, get_TDS(testing::_))
        .WillOnce(DoAll(testing::SetArgReferee<0>(0.0f), Return(MixingDevice::ErrorCode::TDS_READ_ERROR)));
    EXPECT_CALL(mixingDevice, controlNutrientMixingValve(false)).Times(1);
    mixingManager.run();
}
