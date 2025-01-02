#ifndef TIME_SERVER_MOCK_HPP
#define TIME_SERVER_MOCK_HPP

#include "gmock/gmock.h"
#include "time.hpp"

class MockTimeServer : public TimeServer {
   public:
    MOCK_METHOD(utime_t, getUtimeUs, (), (override));
    MOCK_METHOD(bool, getUClockUs, (utime_t&), (override));
};

#endif  // TIME_SERVER_MOCK_HPP