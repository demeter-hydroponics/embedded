#include "CppUTest/TestHarness.h"

extern "C" {
#include "math.h"
}

TEST_GROUP(dummy_test){void setup(){} void teardown(){}};

TEST(dummy_test, dummy_test) { CHECK_EQUAL(1, 1); }
