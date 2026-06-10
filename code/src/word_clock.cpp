#include "word_clock.h"

#include <Arduino.h>
#include <FastLED.h>
#include <TimeLib.h>

#include "fl/hsv16.h"
#include "fx/2d/animartrix.hpp"
#include "happy.h"
#include "newyear_sparkling.h"
#include "settings.h"

#define FLOW_STEP_SIZE 5.0
#define KEY_FACE_COLOR "clck_fc"
#define KEY_TICK_COLOR "clck_tc"
#define KEY_TICK_SHOW "clck_tv"
#define KEY_TICK "clck_tick"
#define KEY_FLOW_TIME "clck_ft"
#define KEY_FLOW "clck_flow"
#define KEY_FACE "clck_face"
#define KEY_SENSOR "clck_sens"
#define KEY_ANIM "clck_anim"
#define KEY_SPEED "clck_speed"
#define KEY_STATUS "clck_status"

void WordClock::setAnimation(int value) {
    animation = value;
    persistentDataDirty = true;
    for (const auto& entry : fl::ANIMATION_TABLE) {
        if (entry.anim == animation) Serial.printf("Animation: %s\n", entry.name);
    }
}

void WordClock::update(long now) {
    // Called every second!

    // Serial.printf("UTC = %02d:%02d:%02d\n", hour(moment), minute(moment), second(moment));
    // Serial.printf("LOCAL = %02d:%02d:%02d\n", hour(now), minute(now), second(now));
    // Serial.printf("DST = %d\n", tz.utcIsDST(moment));

    // New year's countdown
    if (newYearsEve(now)) {
        LedArray target;
        segments.countDown(60 - second(now));
        fillTarget(&target, CRGB::WhiteSmoke);
        active.blendTo(&target);
    } else if (newYear(now)) {
        showingFireworks = true;
        uint8_t brightness = FastLED.getBrightness();
        NewYearSparkling(&active, 5 * 60 * 1000).show();
        showFinal(FlowOption::None, brightness);
    } else if (visible) {
        showingFireworks = false;
        if (segments.calculate(hour(now), minute(now), second(now))) {
            updateClockFace(now);
            savePersistentData();
            updateStatusBar(now, true);  // Refresh every minute
        }
        updateTicks(now % 15);
    }

    // Serial.printf("%02d:%02d:%02d\n", hour(now), minute(now), second(now));
}

void WordClock::savePersistentData() {
    if (!persistentDataDirty) return;

    if (prefs.begin(CLOCK_NAMESPACE, false)) {
        prefs.putULong(KEY_FACE_COLOR, customColor.as_uint32_t());
        prefs.putULong(KEY_TICK_COLOR, tickColor.as_uint32_t());
        prefs.putUShort(KEY_TICK, tick.getValue());
        prefs.putUInt(KEY_FLOW_TIME, flowTime);
        prefs.putUShort(KEY_FLOW, flow.getValue());
        prefs.putUShort(KEY_FACE, face.getValue());
        prefs.putBool(KEY_SENSOR, lightSensorActive);
        prefs.putInt(KEY_SPEED, animateSpeed * 10);
        prefs.putInt(KEY_ANIM, animation);
        prefs.putUShort(KEY_STATUS, status.getValue());
        persistentDataDirty = false;
        prefs.end();
    } else
        Serial.println("Could not save: clock prefs not opened (write).");
}

void WordClock::updateTicks(int secondsPassed) {
    int spot = segments.getBlinkSpot();
    for (size_t i = 0; i < dots.size; i++)
        if (i != spot) active.set(dots.data[i], CRGB::Black);

    switch (tick.getValue()) {
        case TickOption::Blink:
            active.set(spot, tickColor);
            FastLED.show();
            delay(50);
            active.set(spot, CRGB::Black);
            FastLED.show();
            break;
        case TickOption::Solid:
            active.set(spot, tickColor);
            FastLED.show();
            break;
        case TickOption::Fade:
            active.set(spot, CRGB(tickColor).fadeToBlackBy(secondsPassed * 255 / 15));
            FastLED.show();
            break;
        case TickOption::Off:
            for (size_t i = 0; i < dots.size; i++) active.set(dots.data[i], CRGB::Black);
            FastLED.show();
            break;
    }
}

void WordClock::updateStatusBar(long now, boolean force) {
    if (status.updateTime(day(now), month(now)) || force) {
        for (int i = 0; i < SIGN_LEDS; i++) {
            CRGB c = status.getColorFor(i, tickColor, customColor);
            setStatusColor(i, c.r, c.g, c.b);
        }
    };

    if (status.isAnimated()) {
        CRGB current[SIGN_LEDS];
        for (int i = 0; i < SIGN_LEDS; i++) {
            current[i] = statusColors[i];
            setStatusColor(i, 0, 0, 0);
        }
        delay(100);
        for (int i = 0; i < SIGN_LEDS; i++) {
            setStatusColor(i, current[i].r, current[i].g, current[i].b);
        }
    }
}

void WordClock::updateClockFace(long now) {
    // Retrieve it first, since random might affect the value
    flow.deRandomize();
    int flowValue = flow.getValue();
    uint8_t brightness = FastLED.getBrightness();

    if (!lightSensorActive) {
        // Reset brightness to custom color.
        brightness = rgb2hsv_approximate(customColor).value;
        FastLED.setBrightness(brightness);
    }

    int today = day(now) * 100 + month(now);
    for (int day : birthdays)
        if (day == today)  //
            flowValue = FlowOption::BirthDay;

    showTransition(flowValue);
    showFinal(flowValue, brightness);
}

void WordClock::initialize() {
    active.clear();
    FastLED.show();

    // Read settings from storage
    if (prefs.begin(CLOCK_NAMESPACE, true)) {
        customColor = CRGB(prefs.getULong(KEY_FACE_COLOR, customColor.as_uint32_t()));
        tickColor = CRGB(prefs.getULong(KEY_TICK_COLOR, tickColor.as_uint32_t()));
        flowTime = prefs.getUInt(KEY_FLOW_TIME, flowTime);
        flow = FlowOption(prefs.getUShort(KEY_FLOW, flow.getValue()));
        face = FaceOption(prefs.getUShort(KEY_FACE, face.getValue()));
        tick = TickOption(prefs.getUShort(KEY_TICK, tick.getValue()));
        lightSensorActive = prefs.getBool(KEY_SENSOR, true);
        animateSpeed = (float)prefs.getInt(KEY_SPEED, 10) / 10.0;
        animation = prefs.getInt(KEY_ANIM, -1);
        status = prefs.getUShort(KEY_STATUS, status.getValue());
        prefs.end();
    } else
        Serial.println("Could not read: clock prefs not opened (read).");
}

void showAniMatrix(LedArray* array, int anim, float speed, int flowTime) {
    // https://github.com/FastLED/FastLED/blob/master/examples/Animartrix/Animartrix.ino

    time_t stopAt = millis() + flowTime;
    XYMap xyMap = XYMap::constructSerpentine(kMatrixWidth, kMatrixHeight);
    fl::Animartrix animartrix(xyMap, static_cast<AnimartrixAnim>(anim % NUM_ANIMATIONS));
    fl::FxEngine fxEngine(kMatrixWidth * kMatrixHeight);
    fxEngine.addFx(animartrix);
    fxEngine.setSpeed(speed);

    // Serpetine (connect over short part) is by default
    // To change animation: animartrix.fxSet(fxIndex);

    do {
        array->render(&fxEngine);
        FastLED.show();
    } while (millis() < stopAt);
}

void WordClock::showBirthday() {
    Happy happy;

    const int trim = 2;  // Keep not 3, but 1 dark columns between characters.
    int totalSteps = (happy.sizeW1 + happy.sizeW2) * (8 - trim) + kMatrixWidth;
    int step = totalSteps;
    int shifts = 0;

    while (step >= 0) {
        int columnStart = max(0, kMatrixWidth - 1 - shifts);
        int wordStart = max(0, shifts - kMatrixWidth + 1);
        // Serial.printf("Column: %d, Word1start: %d!\n", columnStart, word1Start);

        active.clear();

        // Go through all columns of the matrix
        for (size_t column = columnStart; column < kMatrixWidth; column++) {
            // Find correct bit where each character has 8 bits, but is 5 bits wide.
            int offset = 0;
            int bitPlace = 7 - (wordStart + column - columnStart);
            while (bitPlace < trim) {
                bitPlace += (8 - trim);
                offset += FONT_HEIGHT;
            }
            for (size_t row = 0; row < FONT_HEIGHT; row++) {
                int item_l1 = row + offset;
                int item_l2 = row + offset - happy.sizeW1 * FONT_HEIGHT;
                bool on_l1 = item_l1 < happy.sizeW1 * FONT_HEIGHT && (1 & (happy.word1[item_l1] >> bitPlace)) > 0;
                bool on_l2 = item_l2 >= 0 && item_l2 < happy.sizeW2 * FONT_HEIGHT && (1 & (happy.word2[item_l2] >> bitPlace)) > 0;
                CRGB color_l1 = active.get(Segments::XY(column, row));
                CRGB color_l2 = active.get(Segments::XY(column, row + 6));
                if (on_l1) color_l1 = CRGB::Green3;
                if (on_l2) color_l2 = CRGB::Red3;
                active.set(Segments::XY(column, row), color_l1);
                active.set(Segments::XY(column, row + 6), color_l2);
            }
        }
        FastLED.show();
        delay(110);
        step--;
        shifts++;
    }
}

void WordClock::showTransition(int flowValue, bool force) {
    if (!visible && !force) return;

    if (!force) future.copyFrom(&active);

    // For dimming: dim8_raw(in.r); are the old gamma correction functions, and
    // https://github.com/FastLED/FastLED/wiki/Pixel-reference#dimming-and-brightening-colors

    /* Optionally add:
        15 x.x CALEIDO3
        17 x.x CALEIDO1
        20 1.5 WAVES
        27 0.5 COMPLEX_KALEIDO_4
        40 0.7 MODULE_EXPERIMENT9
        42 0.7 MODULE_EXPERIMENT_SM1
        45 0.7 MODULE_EXPERIMENT_SM4
        48 1.2 MODULE_EXPERIMENT_SM8
        49 x.x MODULE_EXPERIMENT_SM9
    */

    switch (flowValue) {
        case FlowOption::Mill:
            // fl::AnimartrixAnim::CHASING_SPIRALS
            showAniMatrix(&active, 21, 1.3, flowTime);
            break;

        case FlowOption::Wave:
            // fl::AnimartrixAnim::POLAR_WAVES
            showAniMatrix(&active, 5, 1, flowTime);
            break;

        case FlowOption::Spiral:
            // fl::AnimartrixAnim::SPIRALUS2
            showAniMatrix(&active, 10, 1, flowTime);
            break;

        case FlowOption::Fire:
            // fl::AnimartrixAnim::MODULE_EXPERIMENT3
            showAniMatrix(&active, 34, 1.5, flowTime);
            break;

        case FlowOption::Rings:
            // fl::AnimartrixAnim::RINGS
            showAniMatrix(&active, 23, 0.6, flowTime);
            break;

        case FlowOption::Animate: {
            if (animation < 0 || animation >= AnimartrixAnim::NUM_ANIMATIONS) {
                animation = random(0, fl::AnimartrixAnim::NUM_ANIMATIONS);
                for (const auto& entry : ANIMATION_TABLE) {
                    if (entry.anim == animation) Serial.printf("Animation: %s\n", entry.name);
                }
            }
            showAniMatrix(&active, animation, animateSpeed, flowTime);

        } break;

        case FlowOption::Fade:
            active.fade(flowTime);
            break;

        case FlowOption::FlowDown: {
            int columns[kMatrixWidth];
            int size = kMatrixWidth * kMatrixHeight;
            int column;
            CRGB last;

            for (size_t i = 0; i < kMatrixWidth; i++) columns[i] = kMatrixHeight;

            while (size > 0) {
                last = CRGB::Black;
                // Randomly pick a non-empty column
                do {
                    column = random(0, kMatrixWidth);
                } while (columns[column] <= 0);
                // Serial.printf("Column = %d (still has %d). Steps = %d\n", column, columns[column], size);

                // In this column, shift everything down
                for (size_t i = 0; i < kMatrixHeight; i++) {
                    int pos = Segments::XY(column, i);
                    CRGB currentColor = active.get(pos);
                    active.set(pos, last);
                    last = currentColor;
                }

                // Update administration
                columns[column]--;
                size--;

                // Show new and do a random delay
                FastLED.show();
                delay(random(0, flowTime / 250));
            }
        } break;

        case FlowOption::Swipe: {
            // Starts at bottom, goes to top
            int bottom = kMatrixHeight - 1;
            for (size_t y = 0; y < bottom; y++) {
                size_t line = bottom - y - 1;
                for (size_t x = 0; x < kMatrixWidth; x++) {
                    int led = line * kMatrixHeight + x;
                    active.set(led, tickColor);
                    if (line + 1 < bottom) active.set(led + kMatrixWidth, CRGB::Black);
                }
                FastLED.show();
                delay(flowTime / (2 * bottom));
            }
            active.clear();
            FastLED.show();
        } break;

        case FlowOption::Scanner: {
            // Starts right, goes left
            for (size_t xt = 0; xt < kMatrixWidth - 1; xt++) {
                int x = kMatrixWidth - xt - 1;
                for (size_t y = 0; y < kMatrixHeight - 1; y++) {
                    int led = Segments::XY(x, y);
                    active.set(Segments::XY(x, y), tickColor);
                    if (x + 1 < kMatrixHeight) active.set(Segments::XY(x + 1, y), CRGB::Black);
                }
                FastLED.show();
                delay(flowTime / (2 * kMatrixWidth));
            }
            active.clear();
            FastLED.show();
        } break;

        case FlowOption::BirthDay:
            showBirthday();
            break;
    }

    // Make sure the status leds are set back to original
    // If we want to blend from pattern to target, copy only status leds from future state
    if (!force) active.copyFrom(&future);
    active.clearClockFace();
    FastLED.show();
}

void WordClock::animate() {
    if (!visible) return;

    EVERY_N_MILLISECONDS(100) {
        if (face.getValue() == FaceOption::Animate) {
            int rainbowPosition = 0;
            int activateCount = segments.getActiveDigits();
            CHSV hsv = rgb2hsv_approximate(customColor);

            for (int i = 0; i < FACE_LEDS; i++) {
                int pixelPosition = XYTable[i];
                if (segments.isActive(pixelPosition)) {
                    uint8_t hue = map(rainbowPosition, 0, activateCount - 1, 0 + colorOffset, 255 + colorOffset);
                    active.set(pixelPosition, CRGB(CHSV(hue % 256, hsv.sat, hsv.val)));
                    rainbowPosition++;
                }
            }
            colorOffset -= 1;
            if (colorOffset < 0) colorOffset = 255;
            FastLED.show();
        }
    }
}

void WordClock::showFinal(int flowValue, int previousBrightness, bool force) {
    if (!visible && !force) return;

    LedArray target;

    // For Rainbow: use the saturation of the custom color
    CHSV hsv = rgb2hsv_approximate(customColor);
    int activateCount = segments.getActiveDigits();
    int rainbowPosition = 0;

    // For ColorWord or ColorChar
    byte marker = 0;
    bool inWord = false;
    CRGB wordColor = CRGB::Black;

    // Final face of clock is set in the chosen colors in target
    switch (face.getValue()) {
        case FaceOption::Custom:
            fillTarget(&target, customColor);
            break;

        case FaceOption::Rainbow:
        case FaceOption::Animate:
            for (int i = 0; i < FACE_LEDS; i++) {
                int pixelPosition = XYTable[i];
                CRGB value = CRGB::Black;
                if (segments.isActive(pixelPosition)) {
                    uint8_t hue = map(rainbowPosition, 0, activateCount - 1, 0 + colorOffset, 255 + colorOffset);
                    value = CRGB(CHSV(hue % 256, hsv.sat, hsv.val));
                    rainbowPosition++;
                }
                target.set(pixelPosition, value);
            }
            break;

        case FaceOption::ColorWord:
            for (int i = 0; i < FACE_LEDS; i++) {
                int pixelPosition = XYTable[i];
                CRGB value = CRGB::Black;
                if (segments.isActive(pixelPosition)) {
                    // Serial.printf("Active pixel %d [%d] %d\n", pixelPosition, i, inWord);
                    if (!inWord) {
                        inWord = true;
                        int pos = random8(8);
                        while ((marker & (1 << pos)) > 0) {
                            // Serial.printf("Fixing duplicate color\n");
                            pos = random(8);
                            // print_byte(marker);
                        }
                        marker = marker | (1 << pos);
                        wordColor = CRGB(CHSV(pos * 32, hsv.sat, hsv.val));
                    }
                    value = wordColor;
                } else {
                    inWord = false;
                    wordColor = CRGB::Black;
                }
                target.set(pixelPosition, value);
            }
            break;

        case FaceOption::ColorChar:
            for (int i = 0; i < FACE_LEDS; i++) {
                int pixelPosition = XYTable[i];
                CRGB value = CRGB::Black;
                if (segments.isActive(pixelPosition)) {
                    int pos = random8(8);
                    while (pos == marker) pos = random(8);
                    marker = pos;
                    value = CRGB(CHSV(pos * 32, hsv.sat, hsv.val));
                }
                target.set(pixelPosition, value);
            }
            break;

        default:
            fillTarget(&target, defaultColor);
            break;
    }

    // Note: we built the new face in the target; Still need to transition to it.
    // Note: setBrightness will only work for LEDs set AFTER the call.

    switch (flowValue) {
        case FlowOption::Fade:
            active.clearClockFace();
            FastLED.setBrightness(previousBrightness);
            active.blendTo(&target, flowTime);
            break;

        case FlowOption::Swipe: {
            int bottom = kMatrixHeight - 1;
            for (size_t y = 0; y < bottom; y++) {
                for (size_t x = 0; x < kMatrixWidth; x++) {
                    int led = y * kMatrixHeight + x;
                    active.set(led, tickColor);
                    if (y > 0) active.set(led - kMatrixWidth, target.get(led - kMatrixWidth));
                }
                FastLED.show();
                delay(flowTime / (2 * bottom));
            }

            for (size_t x = 0; x < kMatrixWidth; x++) {
                int led = (bottom - 1) * kMatrixHeight + x;
                active.set(led, target.get(led));
            }
            FastLED.setBrightness(previousBrightness);
            FastLED.show();
        } break;

        case FlowOption::Scanner: {
            // Back from left to right
            for (size_t x = 0; x < kMatrixWidth; x++) {
                for (size_t y = 0; y < kMatrixHeight - 1; y++) {
                    int led = Segments::XY(x, y);
                    active.set(led, tickColor);
                    if (x > 0) {
                        int prevIndex = Segments::XY(x - 1, y);
                        active.set(prevIndex, target.get(prevIndex));
                    }
                }
                FastLED.show();
                delay(flowTime / (2 * kMatrixWidth));
            }
            for (size_t y = 0; y < kMatrixHeight - 1; y++) {
                int prevIndex = Segments::XY(kMatrixWidth - 1, y);
                active.set(prevIndex, target.get(prevIndex));
            }
            FastLED.setBrightness(previousBrightness);
            FastLED.show();
        } break;

        case FlowOption::None:
            FastLED.setBrightness(previousBrightness);
            active.blendTo(&target);
            break;

        default:
            FastLED.setBrightness(previousBrightness);
            active.blendTo(&target, 1000);
            break;
    }
}

/**
 * Move segment to target
 */
void WordClock::fillTarget(LedArray* data, CRGB color) {
    for (int i = 0; i < FACE_LEDS; i++) {
        CRGB value = CRGB::Black;
        if (segments.isActive(i)) value = color;
        data->set(i, value);
    }
}

void WordClock::setStatusPattern(int value) {
    status = StatusOption(value);
    for (int i = 0; i < SIGN_LEDS; i++) {
        CRGB c = status.getColorFor(i, tickColor, customColor);
        setStatusColor(i, c.r, c.g, c.b);
    }
}

void WordClock::resetSettings() {
    prefs.clear();
    // Show all leds
    for (size_t i = 0; i < NUM_LEDS; i++) {
        active.set(i, CRGB::WhiteSmoke);
        FastLED.show();
        delay(25);
        active.set(i, CRGB::Black);
    }
    FastLED.show();
}
