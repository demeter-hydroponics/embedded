#ifndef TIME_SERVER_MOCK_HPP
#define TIME_SERVER_MOCK_HPP

#include "gmock/gmock.h"
#include "time.hpp"

class TimeServerMock : public TimeServer {
   public:
    MOCK_METHOD(utime_t, getUtimeUs, (), (override));
};

#endif  // TIME_SERVER_MOCK_HPP