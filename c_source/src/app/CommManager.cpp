#include "CommManager.hpp"

#include <cstdint>
#include <cstring>

#include "CommManagerTypes.hpp"
#include "MessageQueue.hpp"
#include "TransportLayer.hpp"
#include "common.pb.h"
#include "node/commands.pb.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

#define HANDLE_MESSAGE(command_type, queue)                         \
    {                                                               \
        command_type command = command_type##_init_zero;            \
        if (pb_decode(&istream, command_type##_fields, &command)) { \
            if ((queue) != nullptr) {                               \
                (queue)->send(command);                             \
            }                                                       \
        } else {                                                    \
            ret = false;                                            \
        }                                                           \
    }

CommManager::CommManager(TransportLayer& transport_layer, MessageQueue<CommManagerQueueData_t>& send_message_queue,
                         MessageQueue<SetPumpStateCommand>* set_pump_state_command_queue,
                         MessageQueue<SetPPFDReferenceCommand>* ppfd_command_queue,
                         MessageQueue<SetMixingStateCommand>* set_mixing_state_command_queue,
                         MessageQueue<SetWaterLevelControllerStateCommand>* water_level_controller_state_command_queue)
    : transport_layer_(transport_layer),
      message_queue_(send_message_queue),
      set_pump_state_command_queue_(set_pump_state_command_queue),
      ppfd_command_queue_(ppfd_command_queue),
      set_mixing_state_command_queue_(set_mixing_state_command_queue),
      water_level_controller_state_command_queue_(water_level_controller_state_command_queue) {}

bool CommManager::process_message(MessageChannels channel, uint8_t* buf, size_t len) {
    pb_istream_t istream = pb_istream_from_buffer(buf, len);
    bool ret = true;
    // Decode the message
    switch (channel) {
        case MessageChannels_SET_PUMP_STATE_COMMAND: {
            HANDLE_MESSAGE(SetPumpStateCommand, set_pump_state_command_queue_);
            break;
        }
        case MessageChannels_GROW_LIGHT_PPFD_REFERENCE_COMMAND: {
            HANDLE_MESSAGE(SetPPFDReferenceCommand, ppfd_command_queue_);
            break;
        }
        case MessageChannels_SET_MIXING_STATE_COMMAND: {
            HANDLE_MESSAGE(SetMixingStateCommand, set_mixing_state_command_queue_);
            break;
        }
        case MessageChannels_SET_WATER_LEVEL_CONTROLLER_STATE_COMMAND: {
            HANDLE_MESSAGE(SetWaterLevelControllerStateCommand, water_level_controller_state_command_queue_);
            break;
        }
        default:
            break;
    }

    return ret;
}

void CommManager::run() {
    // clear the buffer
    memset(static_cast<void*>(packet_buffer), 0, sizeof(packet_buffer));
    bytes_written_to_packet_ = 0;

    bool success = true;

    CommManagerQueueData_t data;
    while (message_queue_.receive(data)) {
        uint8_t* buffer = static_cast<uint8_t*>(packet_buffer) + bytes_written_to_packet_;
        // first, figure out how many bytes we'd take up if we encoded the message
        size_t header_size_to_encode = 0U;
        success &= pb_get_encoded_size(&header_size_to_encode, MessageHeader_fields, &data.header);
        const size_t message_size = data.header.length + header_size_to_encode;
        if ((bytes_written_to_packet_ + message_size >= TOTAL_PACKET_BUFFER_SIZE) && success) {
            // we've run out of space in the packet buffer, send the packet
            send_packet();
        }

        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(CommManagerQueueData_t) - bytes_written_to_packet_);
        success &= pb_encode(&ostream, MessageHeader_fields, &data.header);
        bytes_written_to_packet_ += ostream.bytes_written;

        memcpy(buffer + ostream.bytes_written, static_cast<uint8_t*>(data.data), data.header.length);
        bytes_written_to_packet_ += data.header.length;
    }

    if (bytes_written_to_packet_ > 0) {
#ifndef UNIT_TEST
        const bool should_send_packet_after_downsample = (packet_buffer_downsample_index % PACKET_BUFFER_DOWNSAMPLE_FACTOR == 0);

        if (should_send_packet_after_downsample)
#endif
        {
            send_packet();
        }

        packet_buffer_downsample_index++;
    }

    uint8_t commandBuffer[CommManager::COMM_MANAGER_MAX_RX_PACKET_SIZE] = {0};
    size_t bytes_received = 0;

    do {
        bytes_received = transport_layer_.receive(static_cast<uint8_t*>(commandBuffer), sizeof(commandBuffer));
        size_t bytes_processed = 0;
        pb_istream_t istream = pb_istream_from_buffer(static_cast<uint8_t*>(commandBuffer), MessageHeader_size);

        while (bytes_processed < bytes_received) {
            // Decode the message header
            MessageHeader header = MessageHeader_init_zero;

            if (pb_decode(&istream, MessageHeader_fields, &header) == false) {
                break;
            }
            bytes_processed += MessageHeader_size;

            if (process_message(header.channel, static_cast<uint8_t*>(commandBuffer) + bytes_processed, header.length) == false) {
                break;
            }
            bytes_processed += header.length;
        }

    } while (bytes_received > 0);
}

void CommManager::send_packet() {
    transport_layer_.send(static_cast<uint8_t*>(packet_buffer), bytes_written_to_packet_);
    reset_buffer();
}

void CommManager::reset_buffer() {
    memset(static_cast<uint8_t*>(packet_buffer), 0, sizeof(packet_buffer));
    bytes_written_to_packet_ = 0;
}