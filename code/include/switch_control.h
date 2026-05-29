#ifndef SWITCH_CONTROL_h
#define SWITCH_CONTROL_h

#include <Arduino.h>

#include "wifi_connect.h"
#include "word_clock.h"

#define PIN_SELECT 4
#define PIN_NEXT 5

#define DEBOUNCE_TIME 50
#define SETTINGS_COUNT 5

class SwitchControl {
   private:
    WordClock* clockRef = NULL;
    CRGB passive = CRGB::DarkRed;
    CRGB active = CRGB::DarkGreen;
    CRGB s[SETTINGS_COUNT] = {passive, passive, passive, passive, passive};
    bool shown;
    bool settingsActive = false;
    bool resetActive = false;
    int8_t brightness = 125;

    // CRGB settingsMap[FACE_LEDS];
    // uint8_t settingsIndex = 0;

   public:
    SwitchControl() {
        // config input pins and enable the internal pull-up resistor
        // Switches connect to GROUND! Normally OPEN means HIGH.

        pinMode(PIN_SELECT, INPUT_PULLUP);
        pinMode(PIN_NEXT, INPUT_PULLUP);
    }

    void setup(WordClock* clock) { clockRef = clock; }

    void loop() {
        if (settingsActive == false && digitalRead(PIN_NEXT) == LOW) {
            resetActive = true;

            brightness = FastLED.getBrightness();
            FastLED.setBrightness(255);
            clockRef->setVisible(false);
            shown = false;
            clockRef->markIndication(0, CRGB::Green2);
            FastLED.show();

            delay(2000);
            if (digitalRead(PIN_NEXT) == LOW && digitalRead(PIN_SELECT) == LOW) {
                clockRef->markIndication(0, CRGB::Orange2);
                FastLED.show();
                delay(3000);
                if (digitalRead(PIN_NEXT) == LOW && digitalRead(PIN_SELECT) == LOW) {
                    clockRef->markIndication(0, CRGB::Red2);
                    FastLED.show();
                    wifiReset();
                    clockRef->resetSettings();
                    resetActive = false;
                    ESP.restart();
                } else
                    resetActive = false;
            } else
                resetActive = false;

            if (resetActive == false) {
                FastLED.setBrightness(brightness);
                clockRef->setVisible(true);
            }
        }

        if (resetActive == false && digitalRead(PIN_SELECT) == LOW) {
            settingsActive = true;
            // Settings mode is entered
            brightness = FastLED.getBrightness();
            FastLED.setBrightness(255);
            clockRef->setVisible(false);
            shown = false;

            size_t index = 0;
            while (index < SETTINGS_COUNT) index = readSetting(index);

            // Settings are set, now update
            updateSettings(FlowOption(clockRef->getFlowOption()),  //
                           FaceOption(clockRef->getFaceOption()),  //
                           clockRef->getTickState(),               //
                           clockRef->getLightSensorState(),        //
                           clockRef->getFlowTime());

            settingsActive = false;
            FastLED.setBrightness(brightness);
            clockRef->setVisible(true);
        }
    }

   private:
    void clearMarkings() {
        for (size_t i = 0; i < kMatrixWidth; i++) clockRef->markIndication(i, CRGB::Black);
        FastLED.show();
    }

    void updateLightSensor() {
        clockRef->markIndication(7, clockRef->getLightSensorState() ? CRGB::White : CRGB::DarkBlue);  // TWAA_L_F
        FastLED.show();
    }

    void updateTickSensor() {
        clockRef->markIndication(3, clockRef->getTickState() ? CRGB::White : CRGB::DarkBlue);  // _T_WAALF
        FastLED.show();
    }

    void updateFlowTime() {
        int currentTime = clockRef->getFlowTime();
        // 13 positions, for time between 250 and 3000
        int pos = max(0, ((currentTime / 250) - 1) % kMatrixWidth);
        if (pos >= kMatrixWidth) pos = 0;

        clearMarkings();
        clockRef->markIndication(pos, CRGB::White);
        FastLED.show();
    }

    /*
    void showHue() {
        CHSV customColor = rgb2hsv_approximate(clockRef->getCustomColor());
        uint8_t sat = customColor.sat;
        uint8_t val = customColor.val;
        uint8_t current = customColor.hue;
        uint8_t delta = 255;

        for (size_t i = 0; i < FACE_LEDS; i++) {
            uint8_t hue = map(i, 0, FACE_LEDS - 1, 0, 255);
            settingsMap[i] = CRGB(CHSV(hue, sat, val));
            uint8_t diff = abs(current - hue);
            if (diff < delta) {
                delta = diff;
                settingsIndex = i;
            }
        }
        Serial.printf("Set hue index to %d (diff = %d)\n", settingsIndex, delta);
        clockRef->showMap(settingsMap, FACE_LEDS, settingsIndex);
    }

    void showSaturation() {
        CHSV customColor = rgb2hsv_approximate(clockRef->getCustomColor());
        uint8_t current = customColor.sat;
        uint8_t val = customColor.val;
        uint8_t hue = customColor.hue;
        uint8_t delta = 255;

        for (size_t i = 0; i < FACE_LEDS; i++) {
            uint8_t sat = map(i, 0, FACE_LEDS - 1, 0, 255);
            settingsMap[i] = CRGB(CHSV(hue, sat, val));
            uint8_t diff = abs(current - sat);
            if (diff < delta) {
                delta = diff;
                settingsIndex = i;
            }
        }
        Serial.printf("Set sat index to %d (diff = %d)\n", settingsIndex, delta);
        clockRef->showMap(settingsMap, FACE_LEDS, settingsIndex);
    }

    void showValue() {
        CHSV customColor = rgb2hsv_approximate(clockRef->getCustomColor());
        uint8_t sat = customColor.sat;
        uint8_t current = customColor.val;
        uint8_t hue = customColor.hue;
        uint8_t delta = 255;

        for (size_t i = 0; i < FACE_LEDS; i++) {
            uint8_t val = map(i, 0, FACE_LEDS - 1, 0, 255);
            settingsMap[i] = CRGB(CHSV(hue, sat, val));
            uint8_t diff = abs(current - val);
            if (diff < delta) {
                delta = diff;
                settingsIndex = i;
            }
        }
        Serial.printf("Set val index to %d (diff = %d)\n", settingsIndex, delta);
        clockRef->showMap(settingsMap, FACE_LEDS, settingsIndex);
    }

    CRGB nextColor() {
        settingsIndex = (settingsIndex + 1) % 256;
        return settingsMap[settingsIndex];
    }
*/

    size_t readSetting(size_t index) {
        // Update indicator
        for (size_t i = 0; i < SETTINGS_COUNT; i++) s[i] = passive;
        s[index] = active;
        clearMarkings();
        // clockRef->showSettings(s[0], s[1], s[2], s[3], s[4]);
        clockRef->showSettings(SETTINGS_COUNT, s);

        // Wait for update on setting
        while (digitalRead(PIN_NEXT) != LOW) {
            delay(DEBOUNCE_TIME);
            switch (index) {
                case 0:  // Change Face
                    if (!shown) {
                        clockRef->showFinal(FlowOption::None, FastLED.getBrightness(), true);
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setNextFace();
                    }
                    break;
                case 1:  // Change Flow
                    if (!shown) {
                        clockRef->showTransition(clockRef->getFlowOption(), true);
                        clockRef->showSettings(SETTINGS_COUNT, s);
                        shown = true;
                    }
                    // Don't show transition beforehand, otherwise it keeps on going
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setNextFlow();
                        // Flow will change the status leds
                        clockRef->showSettings(SETTINGS_COUNT, s);
                        // clockRef->showSettings(s[0], s[1], s[2], s[3], s[4]);
                    }
                    break;
                case 2:  // Set light sensor
                    if (!shown) {
                        clockRef->clearClockFace();
                        updateLightSensor();
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setLightSensorState(!clockRef->getLightSensorState());
                        updateLightSensor();
                    }
                    break;
                case 3:
                    if (!shown) {
                        clockRef->clearClockFace();
                        updateTickSensor();
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setTickVisible(!clockRef->getTickState());
                        updateTickSensor();
                    }
                    break;
                case 4:
                    if (!shown) {
                        clockRef->clearClockFace();
                        updateFlowTime();
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setFlowTime((clockRef->getFlowTime() + 250) % 3500);
                        updateFlowTime();
                    }
                    break;
                    /*
                    // These options seem to mangle the LEDs for showing 'noise'.
                case 5:
                    if (!shown) {
                        showHue();
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setColor(nextColor(), true);
                        showHue();
                    }
                    break;
                case 6:
                    if (!shown) {
                        showSaturation();
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setColor(nextColor(), true);
                        showSaturation();
                    }
                    break;
                case 7:
                    if (!shown) {
                        showValue();
                        shown = true;
                    }
                    if (digitalRead(PIN_SELECT) == LOW) {
                        clockRef->flashSignal(index, CRGB::Blue);
                        clockRef->setColor(nextColor(), true);
                        showValue();
                    }
                    break;
                    */
            }
        }
        index++;
        while (digitalRead(PIN_NEXT) != HIGH) delay(DEBOUNCE_TIME);
        shown = false;
        return index;
    }
};

#endif
