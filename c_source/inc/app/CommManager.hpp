#ifndef COMM_MANAGER_HPP
#define COMM_MANAGER_HPP

#include "CommManagerTypes.hpp"
#include "MessageQueue.hpp"
#include "TransportLayer.hpp"
#include "time.hpp"

class CommManager {
   public:
    /**
     * @brief Construct a new Comm Manager object
     *
     * @param transport_layer The transport layer to use
     */
    CommManager(TransportLayer& transport_layer, MessageQueue<CommManagerQueueData_t>& message_queue);

    /**
     * @brief Run the communication manager
     *
     */
    void run();

   private:
    TransportLayer& transport_layer_;
    MessageQueue<CommManagerQueueData_t>& message_queue_;

    void send_packet();

    constexpr static size_t COMM_MANAGER_MAX_MESSAGES_IN_PACKET = 20;
    constexpr static size_t TOTAL_PACKET_BUFFER_SIZE = sizeof(CommManagerQueueData_t) * COMM_MANAGER_MAX_MESSAGES_IN_PACKET;

    uint8_t packet_buffer[TOTAL_PACKET_BUFFER_SIZE] = {0};
    size_t bytes_written_to_packet = 0;
};

#endif  // COMM_MANAGER_HPP