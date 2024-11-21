#ifndef TRANSPORT_LAYER_HPP
#define TRANSPORT_LAYER_HPP
#include <stddef.h>
#include <stdint.h>

class TransportLayer {
   public:
    /**
     * @brief Send a buffer of data over the transport layer
     * @return True if the send was successful
     */
    virtual bool send(const uint8_t* buffer, size_t length) = 0;

    /**
     * @brief Receive a buffer of data over the transport layer
     * @return The number of bytes received
     */
    virtual size_t receive(uint8_t* buffer, size_t length) = 0;
};

#endif  // TRANSPORT_LAYER_HPP