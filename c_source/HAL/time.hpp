#ifndef TIME_HPP
#define TIME_HPP
#include "stdint.h"

typedef uint64_t utime_t;

class TimeServer {
   public:
    // Returns the current time in microseconds
    virtual utime_t getUtimeUs() = 0;

    /**
     * @brief Get the current, world-clock time in microseconds
     *
     * @param utime The current time in microseconds
     *
     * @return true if the time was successfully retrieved, false otherwise
     */
    virtual bool getUClockUs(utime_t& utime) = 0;

    /**
     * @brief Delay for a given number of microseconds
     *
     * @param delay The number of microseconds to delay
     */
    virtual void delayUs(utime_t delay) = 0;
};

#endif  // TIME_HPP