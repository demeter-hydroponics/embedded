#include <gtest/gtest.h>

#include "MockMessageQueue.hpp"
#include "MockPumpDevice.hpp"
#include "MockTime.hpp"
#include "PumpManager.hpp"
#include "column/commands.pb.h"

using namespace ::testing;

TEST(PumpManager, test_init_to_debug) {
    MockTimeServer timeServer;
    MockMessageQueue<CommManagerQueueData_t> commMessageQueue;
    MockMessageQueue<SetPumpStateCommand> pumpStateCommandQueue;
    MockPumpDevice pump;

    PumpManager pumpManager(timeServer, commMessageQueue, pumpStateCommandQueue, pump);
    EXPECT_EQ(pumpManager.getState(), PumpManager::PumpManagerState::INIT);

    // Send a command through the pumpStateCommandQueue
    SetPumpStateCommand command;
    command.SelectedPump = PumpType_PRIMARY;
    command.State = PumpState_PUMP_ON;
    EXPECT_CALL(pumpStateCommandQueue, receive(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(command), testing::Return(true)));
    pumpManager.run();

    // Check mock expectations
    testing::Mock::VerifyAndClearExpectations(&pumpStateCommandQueue);
    testing::Mock::VerifyAndClearExpectations(&pump);

    EXPECT_EQ(pumpManager.getState(), PumpManager::PumpManagerState::DEBUG);

    // expect a call to the pump device, enabling the primary pump
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, true)).Times(1);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false)).Times(1);

    pumpManager.run();
    testing::Mock::VerifyAndClearExpectations(&pump);

    command.SelectedPump = PumpType_SECONDARY;
    command.State = PumpState_PUMP_ON;
    EXPECT_CALL(pumpStateCommandQueue, receive(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(command), testing::Return(true)));

    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false)).Times(1);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, true)).Times(1);

    pumpManager.run();
    testing::Mock::VerifyAndClearExpectations(&pump);

    command.SelectedPump = PumpType_BOTH;
    command.State = PumpState_PUMP_ON;
    EXPECT_CALL(pumpStateCommandQueue, receive(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(command), testing::Return(true)));

    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, true)).Times(1);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, true)).Times(1);

    pumpManager.run();
    testing::Mock::VerifyAndClearExpectations(&pump);

    command.SelectedPump = PumpType_BOTH;
    command.State = PumpState_PUMP_OFF;
    EXPECT_CALL(pumpStateCommandQueue, receive(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(command), testing::Return(true)));

    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false)).Times(1);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false)).Times(1);
    pumpManager.run();
}

TEST(PumpManager, test_init_to_running_primary) {
    MockTimeServer timeServer;
    MockMessageQueue<CommManagerQueueData_t> commMessageQueue;
    MockMessageQueue<SetPumpStateCommand> pumpStateCommandQueue;
    MockPumpDevice pump;

    PumpManager pumpManager(timeServer, commMessageQueue, pumpStateCommandQueue, pump);

    EXPECT_EQ(pumpManager.getState(), PumpManager::PumpManagerState::INIT);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false)).Times(1);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, true)).Times(1);
    EXPECT_CALL(pump, controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false)).Times(2);
    pumpManager.run();
    EXPECT_EQ(pumpManager.getState(), PumpManager::PumpManagerState::RUNNING_PRIMARY);
}

TEST(PumpManager, test_running_primary_to_debug) {
    MockTimeServer timeServer;
    MockMessageQueue<CommManagerQueueData_t> commMessageQueue;
    MockMessageQueue<SetPumpStateCommand> pumpStateCommandQueue;
    MockPumpDevice pump;

    PumpManager pumpManager(timeServer, commMessageQueue, pumpStateCommandQueue, pump);

    pumpManager.run();

    EXPECT_EQ(pumpManager.getState(), PumpManager::PumpManagerState::RUNNING_PRIMARY);
    testing::Mock::VerifyAndClearExpectations(&pump);

    // Send a command through the pumpStateCommandQueue
    SetPumpStateCommand command;
    command.SelectedPump = PumpType_SECONDARY;
    command.State = PumpState_PUMP_ON;

    EXPECT_CALL(pumpStateCommandQueue, receive(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(command), testing::Return(true)));

    pumpManager.run();
    EXPECT_EQ(pumpManager.getState(), PumpManager::PumpManagerState::DEBUG);
}