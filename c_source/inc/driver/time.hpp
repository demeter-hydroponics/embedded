#ifndef TIME_HPP
#define TIME_HPP
#include "stdint.h"

typedef uint64_t utime_t;

class TimeServer {
   public:
    // Returns the current time in microseconds
    virtual utime_t getUtimeUs() = 0;
};

#endif  // TIME_HPP