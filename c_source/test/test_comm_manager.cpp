#include <gtest/gtest.h>
#include <pb_encode.h>

#include "CommManager.hpp"
#include "MockMessageQueue.hpp"
#include "MockTransportLayer.hpp"
#include "column/commands.pb.h"
#include "util.hpp"

using namespace ::testing;

TEST(CommManagerTest, TestCommManagerPollEmptyQueue) {
    MockTransportLayer transport_layer;
    MockMessageQueue<CommManagerQueueData_t> message_queue;
    MockMessageQueue<SetPumpStateCommand> set_pump_state_command_queue;
    CommManager comm_manager(transport_layer, message_queue, &set_pump_state_command_queue, nullptr);

    EXPECT_CALL(transport_layer, send).Times(0);
    EXPECT_CALL(message_queue, receive).Times(1);

    comm_manager.run();
}

TEST(CommManagerTest, TestCommManagerPollQueue) {
    MockTransportLayer transport_layer;
    MockMessageQueue<CommManagerQueueData_t> message_queue;
    MockMessageQueue<SetPumpStateCommand> set_pump_state_command_queue;
    CommManager comm_manager(transport_layer, message_queue, &set_pump_state_command_queue, nullptr);

    CommManagerQueueData_t data;
    data.header.channel = MessageChannels_MIXING_STATS;
    data.header.length = 5;
    data.data[0] = 0x01;
    data.data[1] = 0x02;
    data.data[2] = 0x03;
    data.data[3] = 0x04;
    data.data[4] = 0x05;

    EXPECT_CALL(message_queue, receive).WillOnce(DoAll(SetArgReferee<0>(data), Return(true))).WillRepeatedly(Return(false));
    uint8_t expected_buffer[sizeof(data)] = {0};
    size_t bytes_written = 0;

    pb_ostream_t ostream = pb_ostream_from_buffer(expected_buffer, sizeof(expected_buffer));
    IGNORE(pb_encode(&ostream, MessageHeader_fields, &data.header));
    bytes_written += ostream.bytes_written;
    memcpy(expected_buffer + ostream.bytes_written, data.data, data.header.length);
    bytes_written += data.header.length;

    uint8_t sent_buffer[sizeof(data)] = {0};
    EXPECT_CALL(transport_layer, send(_, bytes_written)).WillOnce(Invoke([&sent_buffer](const uint8_t* buf, int length) {
        memcpy(sent_buffer, buf, length);
        return true;
    }));

    comm_manager.run();

    EXPECT_THAT(sent_buffer, ElementsAreArray(expected_buffer));
}

TEST(CommManagerTest, MultipleElementsInQueue) {
    MockTransportLayer transport_layer;
    MockMessageQueue<CommManagerQueueData_t> message_queue;
    MockMessageQueue<SetPumpStateCommand> set_pump_state_command_queue;
    CommManager comm_manager(transport_layer, message_queue, &set_pump_state_command_queue, nullptr);

    CommManagerQueueData_t data1;
    data1.header.channel = MessageChannels_MIXING_STATS;
    data1.header.length = 5;
    data1.data[0] = 0x01;
    data1.data[1] = 0x02;
    data1.data[2] = 0x03;
    data1.data[3] = 0x04;
    data1.data[4] = 0x05;

    CommManagerQueueData_t data2;
    data2.header.channel = MessageChannels_MIXING_STATS;
    data2.header.length = 3;
    data2.data[0] = 0x06;
    data2.data[1] = 0x07;
    data2.data[2] = 0x08;

    EXPECT_CALL(message_queue, receive)
        .WillOnce(DoAll(SetArgReferee<0>(data1), Return(true)))
        .WillOnce(DoAll(SetArgReferee<0>(data2), Return(true)))
        .WillRepeatedly(Return(false));

    uint8_t expected_buffer[sizeof(data1) + sizeof(data2)] = {0};
    size_t bytes_written = 0;

    pb_ostream_t ostream = pb_ostream_from_buffer(expected_buffer, sizeof(expected_buffer));
    IGNORE(pb_encode(&ostream, MessageHeader_fields, &data1.header));
    bytes_written += ostream.bytes_written;
    memcpy(expected_buffer + ostream.bytes_written, data1.data, data1.header.length);
    bytes_written += data1.header.length;

    ostream = pb_ostream_from_buffer(expected_buffer + bytes_written, sizeof(expected_buffer) - bytes_written);
    IGNORE(pb_encode(&ostream, MessageHeader_fields, &data2.header));
    bytes_written += ostream.bytes_written;
    memcpy(expected_buffer + bytes_written, data2.data, data2.header.length);
    bytes_written += data2.header.length;

    uint8_t sent_buffer[sizeof(data1) + sizeof(data2)] = {0};
    EXPECT_CALL(transport_layer, send(_, bytes_written)).WillOnce(Invoke([&sent_buffer](const uint8_t* buf, int length) {
        memcpy(sent_buffer, buf, length);
        return true;
    }));

    comm_manager.run();

    EXPECT_THAT(sent_buffer, ElementsAreArray(expected_buffer));
}

TEST(CommManagerTest, receive_and_marshall_pump_command) {
    MockTransportLayer transport_layer;
    MockMessageQueue<CommManagerQueueData_t> message_queue;
    MockMessageQueue<SetPumpStateCommand> set_pump_state_command_queue;
    CommManager comm_manager(transport_layer, message_queue, &set_pump_state_command_queue, nullptr);

    uint8_t frame[MessageHeader_size + SetPumpStateCommand_size] = {0};
    pb_ostream_t ostream = pb_ostream_from_buffer(frame, sizeof(frame));

    SetPumpStateCommand command;
    command.State = PumpState_PUMP_ON;
    command.SelectedPump = PumpType_PRIMARY;

    MessageHeader header;
    header.channel = MessageChannels_SET_PUMP_STATE_COMMAND;
    size_t command_size = 0;
    IGNORE(pb_get_encoded_size(&command_size, SetPumpStateCommand_fields, &command));
    header.length = command_size;
    header.timestamp = 0;

    pb_encode(&ostream, MessageHeader_fields, &header);
    pb_encode(&ostream, SetPumpStateCommand_fields, &command);

    // Expect a call to receive a frame from the transport layer
    EXPECT_CALL(transport_layer, receive(_, _))
        .WillOnce(DoAll(SetArrayArgument<0>(frame, frame + ostream.bytes_written), Return(ostream.bytes_written)))
        .WillRepeatedly(Return(0));

    // Expect a call to send the frame to the set pump state command queue
    EXPECT_CALL(set_pump_state_command_queue, send(_)).WillOnce(Invoke([&command](const SetPumpStateCommand& received_command) {
        EXPECT_EQ(received_command.State, command.State);
        EXPECT_EQ(received_command.SelectedPump, command.SelectedPump);
        return true;
    }));

    comm_manager.run();
}