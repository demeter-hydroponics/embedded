#include "CommManager.hpp"

#include <cstdint>
#include <cstring>

#include "CommManagerTypes.hpp"
#include "MessageQueue.hpp"
#include "TransportLayer.hpp"
#include "common.pb.h"
#include "pb.h"
#include "pb_encode.h"

CommManager::CommManager(TransportLayer& transport_layer, MessageQueue<CommManagerQueueData_t>& message_queue)
    : transport_layer_(transport_layer), message_queue_(message_queue) {}

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
        send_packet();
    }
}

void CommManager::send_packet() {
    transport_layer_.send(static_cast<uint8_t*>(packet_buffer), bytes_written_to_packet_);
    memset(static_cast<uint8_t*>(packet_buffer), 0, sizeof(packet_buffer));
    bytes_written_to_packet_ = 0;
}