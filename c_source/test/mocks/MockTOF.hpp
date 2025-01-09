#ifndef MOCK_TOF_HPP
#define MOCK_TOF_HPP

#include <gmock/gmock.h>

#include "TOF.hpp"

class MockTOF : public BaseTOF {
   public:
    MOCK_METHOD(ErrorCode, get_distance_m, (float& distance), (override));
};

#endif  // MOCK_TOF_HPP