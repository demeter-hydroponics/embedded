#ifndef COMM_MANAGER_TYPES_HPP
#define COMM_MANAGER_TYPES_HPP

#include <stddef.h>
#include <stdint.h>

#include "messages/common/common.pb.h"

constexpr size_t COMM_MANAGER_QUEUE_DATA_SIZE = 256;

typedef struct CommManagerQueueData {
    MessageHeader header;
    uint8_t data[COMM_MANAGER_QUEUE_DATA_SIZE];
#ifdef UNIT_TEST
    bool operator==(const CommManagerQueueData& rhs) const {
        return header.channel == rhs.header.channel && header.length == rhs.header.length &&
               memcmp(data, rhs.data, header.length) == 0;
    }
#endif
} CommManagerQueueData_t;

#endif  // COMM_MANAGER_TYPES_HPP