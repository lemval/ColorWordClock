#include <gtest/gtest.h>
// #include <gmock/gmock.h>

#include "word_clock.h"

TEST(Clock, CheckArray) {
    Serial.println("Running TEST");

    Segments s = Segments();
    for (int hour = 0; hour < 24; hour++) {
        for (int min = 0; min < 60; min++) {
            s.calculate(hour, min, 0);
        }
    }
    EXPECT_TRUE(true);
}

// TEST_F(...)

#include <Arduino.h>

void setup() {
    // should be the same value as for the `test_speed` option in "platformio.ini"
    // default value is test_speed=115200
    Serial.begin(115200);

    ::testing::InitGoogleTest();
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock();
}

void loop() {
    // Run tests
    if (RUN_ALL_TESTS())
        ;

    // sleep for 1 sec
    delay(1000);
}

// #include <unity.h>

// void setUp(void) {
//   // set stuff up here
// }

// void tearDown(void) {
//   // clean stuff up here
// }

// void test_led_builtin_pin_number(void) {
//   Serial.println("Running TEST Unity");
//   TEST_ASSERT_EQUAL(13, LED_BUILTIN);
// }
