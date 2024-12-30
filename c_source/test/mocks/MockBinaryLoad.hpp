#ifndef MOCK_BINARY_LOAD_HPP
#define MOCK_BINARY_LOAD_HPP

#include "BinaryLoad.hpp"
#include "gmock/gmock.h"

class MockBinaryLoad : public BaseBinaryLoad {
   public:
    MOCK_METHOD(BinaryLoad::ErrorCode, setEnabled, (bool enable), (override));
    MOCK_METHOD(BinaryLoad::ErrorCode, getCurrent, (float& current), (override));
    MOCK_METHOD(BinaryLoad::ErrorCode, isFaulted, (bool& fault), (override));
    MOCK_METHOD(void, populateProtobufMessage, (BinaryLoadStats & msg), (override));
};

#endif  // MOCK_BINARY_LOAD_HPP