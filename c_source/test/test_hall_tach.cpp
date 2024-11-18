#include <gtest/gtest.h>

#include "HallTach.hpp"

TEST(HallTachTest, TestFirstSampleThenAskingFreqIsInvalid) {
    HallTach hall_tach(0.0f);
    HallTach::State state;
    float freq = 1.0f;

    // verify it's uninitialized
    state = hall_tach.getFrequency(freq, 0U);
    EXPECT_FLOAT_EQ(0.0, freq);
    EXPECT_EQ(HallTach::State::NOT_VALID_UNINITIALIZED, state);

    // Log a toggle event
    hall_tach.logHallTachToggle(HallTach::TachEdge::RISING, 3);
    freq = 1.0f;

    state = hall_tach.getFrequency(freq, 0U);

    // Use EXPECT_EQ to compare the state
    EXPECT_EQ(HallTach::State::NOT_VALID_NO_CHANGE, state);
    EXPECT_FLOAT_EQ(0.0, freq);
}

TEST(HallTachTest, TestTwoSamplesFreqCalcCorrect) {
    HallTach hall_tach(0.0f);
    HallTach::State state;

    hall_tach.logHallTachToggle(HallTach::TachEdge::RISING, 3);

    hall_tach.logHallTachToggle(HallTach::TachEdge::FALLING, 500000 + 3);

    float freq = 0.0f;

    state = hall_tach.getFrequency(freq, 0U);
    EXPECT_EQ(HallTach::State::VALID, state);
    EXPECT_FLOAT_EQ(1.0, freq);

    // Log another toggle event, but falling - the frequency should still be calculated correctly
    hall_tach.logHallTachToggle(HallTach::TachEdge::FALLING, 1000000 + 3);

    freq = 0.0f;
    state = hall_tach.getFrequency(freq, 0U);
    EXPECT_EQ(HallTach::State::VALID, state);
    EXPECT_FLOAT_EQ(2.0, freq);
}

// Tests: Valid to invalid after too long
TEST(HallTachTest, TestMinFreq) {
    HallTach hall_tach(0.5f);
    HallTach::State state;

    hall_tach.logHallTachToggle(HallTach::TachEdge::RISING, 3);

    hall_tach.logHallTachToggle(HallTach::TachEdge::FALLING, 1000000 + 3);

    float freq = 0.0f;

    state = hall_tach.getFrequency(freq, 1000000 + 5);
    EXPECT_EQ(HallTach::State::VALID, state);
    EXPECT_FLOAT_EQ(0.5, freq);

    // Now, get the time at 4000000 microseconds - this should be NOT_VALID_NO_CHANGE
    state = hall_tach.getFrequency(freq, 4000000);
    EXPECT_EQ(HallTach::State::NOT_VALID_NO_CHANGE, state);
}

TEST(HallTachTest, TestMinFreqEnabledAndAskingForBackwardTime) {
    HallTach hall_tach(0.5f);
    HallTach::State state;

    hall_tach.logHallTachToggle(HallTach::TachEdge::RISING, 3);

    hall_tach.logHallTachToggle(HallTach::TachEdge::FALLING, 1000000 + 3);

    float freq = 0.0f;
    state = hall_tach.getFrequency(freq, 0U);
    EXPECT_EQ(HallTach::State::NOT_VALID_UNINITIALIZED, state);
    EXPECT_FLOAT_EQ(0.0, freq);
}

TEST(HallTachTest, TestNegativeDeltaTimeResetsToUninitialized) {
    HallTach hall_tach(0.0f);
    HallTach::State state;

    // Log an initial toggle
    hall_tach.logHallTachToggle(HallTach::TachEdge::RISING, 5000);
    // Log a toggle with a time less than the previous one (negative delta time)
    hall_tach.logHallTachToggle(HallTach::TachEdge::FALLING, 4000);

    float freq = 1.0f;

    // Check that the state resets to uninitialized
    state = hall_tach.getFrequency(freq, 0U);
    EXPECT_EQ(HallTach::State::NOT_VALID_UNINITIALIZED, state);
    EXPECT_FLOAT_EQ(0.0, freq);
}