#ifndef COMM_MANAGER_HPP
#define COMM_MANAGER_HPP

#include "CommManagerTypes.hpp"
#include "MessageQueue.hpp"
#include "TransportLayer.hpp"
#include "column/commands.pb.h"
#include "common.pb.h"
#include "node/commands.pb.h"
#include "time.hpp"

class CommManager {
   public:
    /**
     * @brief Construct a new Comm Manager object
     *
     * @param transport_layer The transport layer to use
     * @param send_message_queue The message queue to use for sending messsages
     * @param set_pump_state_command_queue The message queue to use for sending pump state commands
     * @param ppfd_command_queue The message queue to use for sending PPFD commands
     */
    CommManager(TransportLayer& transport_layer, MessageQueue<CommManagerQueueData_t>& send_message_queue,
                MessageQueue<SetPumpStateCommand>* set_pump_state_command_queue,
                MessageQueue<SetPPFDReferenceCommand>* ppfd_command_queue,
                MessageQueue<SetMixingStateCommand>* set_mixing_state_command_queue);

    /**
     * @brief Run the communication manager
     *
     */
    void run();

    constexpr static size_t COMM_MANAGER_MAX_MESSAGES_IN_PACKET = 50;
    constexpr static size_t COMM_MANAGER_MAX_RX_PACKET_SIZE = 512;

   private:
    TransportLayer& transport_layer_;
    MessageQueue<CommManagerQueueData_t>& message_queue_;
    MessageQueue<SetPumpStateCommand>* set_pump_state_command_queue_;
    MessageQueue<SetPPFDReferenceCommand>* ppfd_command_queue_;
    MessageQueue<SetMixingStateCommand>* set_mixing_state_command_queue_;

    void send_packet();

    bool process_message(MessageChannels channel, uint8_t* buf, size_t len);

    constexpr static size_t TOTAL_PACKET_BUFFER_SIZE = sizeof(CommManagerQueueData_t) * COMM_MANAGER_MAX_MESSAGES_IN_PACKET;

    uint8_t packet_buffer[TOTAL_PACKET_BUFFER_SIZE] = {0};
    size_t bytes_written_to_packet_ = 0;
};

#endif  // COMM_MANAGER_HPP