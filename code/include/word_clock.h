#ifndef WORD_CLOCK_h
#define WORD_CLOCK_h

#include <FastLED.h>
#include <Preferences.h>
#include <Timezone.h>

#include "face_option.h"
#include "fl/math_macros.h"  // Math helper functions and macros
#include "fl/time_alpha.h"   // Time-based alpha/transition effects
#include "flow_option.h"
#include "fx/2d/blend.h"  // 2D blending effects between layers
#include "fx/2d/wave.h"
#include "led_array.h"
#include "segments.h"
#include "status_option.h"

#define CLOCK_NAMESPACE "ColorClock"
#define MIN_BRIGHTNESS 1

class WordClock {
   private:
    Segments segments;
    LedArray active = LedArray(true);
    LedArray future;
    CRGB defaultColor;
    CRGB statusColors[SIGN_LEDS];
    bool showingFireworks = false;
    int colorOffset = 0;

    // Persistent data
    Preferences prefs;
    bool persistentDataDirty = false;
    // Fields to store
    CRGB customColor;
    CRGB tickColor;
    FlowOption flow = FlowOption::None;
    FaceOption face = FaceOption::Warm;
    StatusOption status = StatusOption::Date;

    int flowTime = 100;
    bool showTick = true;
    bool lightSensorActive = true;
    bool visible = true;
    int8_t animation = -1;
    float animateSpeed = 1.0;
    int dimFactor = 100;

   private:
    void updateTicks();
    void updateClockFace(long now = 0);
    void updateStatusBar(long now, boolean force);
    void fillTarget(LedArray* data, CRGB color);
    void savePersistentData();
    void showBirthday();

    boolean newYearsEve(time_t now) { return month(now) == 12 && day(now) == 31 && hour(now) == 23 && minute(now) == 59 && second(now) >= 45; }
    boolean newYear(time_t now) { return month(now) == 1 && day(now) == 1 && hour(now) == 0 && minute(now) == 0 && !showingFireworks; }

   public:
    WordClock() {
        defaultColor = CRGB(214, 194, 111);
        customColor = CRGB(77, 126, 150);
        tickColor = CRGB::Wheat;
        for (size_t i = 0; i < SIGN_LEDS; i++) statusColors[i] = CRGB::AntiqueWhite;

        // Show at startup
        markIndication(10, CRGB::Red3);
        markIndication(11, CRGB::Green3);
        markIndication(12, CRGB::Blue3);
        FastLED.show();
    };

    void initialize();
    void update(long moment);
    void animate();
    void setStatusPattern(int pattern);
    void resetSettings();

    // Public for settings
    void showTransition(int flowValue, bool force = false);
    void showFinal(int flowValue, int previousBrightness, bool force = false);

    CRGB getCustomColor() { return customColor; };
    CRGB getTickColor() { return tickColor; };
    bool getTickState() { return showTick; }
    int getFlowTime() { return flowTime; }
    int getFlowOption() { return flow.getValue(); }
    int getFaceOption() { return face.getValue(); }
    int getStatusOption() { return status.getValue(); }
    bool getLightSensorState() { return lightSensorActive; }
    float getDimFactor() { return dimFactor; }

    void setNextFlow() { setFlowPattern(flow.next(), true); }
    void setNextFace() { setFacePattern(face.next(), true); }
    int getAnimation() { return animation; }
    float getAnimationSpeed() { return animateSpeed; }
    void setAnimation(int value);
    void setDimFactor(int value) { dimFactor = value; }

    void setAnimationSpeed(float value) {
        animateSpeed = value;
        persistentDataDirty = true;
    }

    void setLightSensorState(bool state) {
        lightSensorActive = state;
        persistentDataDirty = true;
    }

    void setFlowPattern(int index, bool force = false) {
        flow = FlowOption(index);
        persistentDataDirty = true;
        showTransition(flow.getValue(), force);
        showFinal(flow.getValue(), FastLED.getBrightness(), force);
    }

    void setFacePattern(int index, bool force = false) {
        face = FaceOption(index);
        persistentDataDirty = true;
        showFinal(FlowOption::None, FastLED.getBrightness(), force);
    };

    void setColor(int r, int g, int b) { setColor(CRGB(r, g, b)); }
    void setColor(CRGB color, bool force = false) {
        customColor = color;
        persistentDataDirty = true;
        if (!force && face.getValue() != FaceOption::Warm) showFinal(FlowOption::None, FastLED.getBrightness());
    }

    void setFlowTime(int newValue) {
        flowTime = min(5000, max(0, newValue));
        persistentDataDirty = true;
    };

    void setTickColor(int r, int g, int b) {
        tickColor = CRGB(r, g, b);
        persistentDataDirty = true;
    }

    void setTickVisible(boolean state) {
        showTick = state;
        persistentDataDirty = true;
    }

    void flashSignal(int stateLed, CRGB color) {
        int spot = segments.getSignSpot(stateLed);
        CRGB current = active.get(spot);
        active.set(spot, CRGB::Black);
        FastLED.show();
        delay(40);
        active.set(spot, color);
        FastLED.show();
        delay(200);
        active.set(spot, CRGB::Black);
        FastLED.show();
        delay(40);
        active.set(spot, current);
        FastLED.show();
    }

    void showSettings(int size, CRGB* values) {
        for (size_t i = 0; i < kMatrixWidth; i++) {
            CRGB color = i < size ? values[i] : CRGB::Black;
            active.set(segments.getSignSpot(i), color);
        }
        FastLED.show();
    }

    void markIndication(int index, CRGB color) { active.set(Segments::XY(index, 11), color); }

    void activateStatusLed(int index, boolean activate) {
        if (!visible) return;
        CRGB color = status.getColorFor(index, tickColor, customColor);
        if (activate) color = statusColors[index];
        if (rgb2hsv_approximate(color).value < 5) color = CRGB::DarkRed;
        int where = segments.getSignSpot(index);
        if (where > 0) {
            active.set(where, color);
            FastLED.show();
        }
    }

    void setStatusColor(int index, int r, int g, int b) {
        if (!visible) return;
        CRGB color = CRGB(r, g, b);
        statusColors[index] = color;
        int where = segments.getSignSpot(index);
        if (where > 0) {
            active.set(where, color);
            FastLED.show();
        }
    }

    void clearClockFace() {
        for (size_t i = 0; i < FACE_LEDS; i++) active.set(i, CRGB::Black);
        FastLED.show();
    }

    void setVisible(boolean activate) {
        // Also sent when brightness/color changes
        if (activate == visible) return;

        if (activate) {
            visible = true;
            active.copyFrom(&future);
            updateClockFace();
        } else {
            visible = false;
            future.copyFrom(&active);
            active.clear();
            FastLED.show();
        }
    }

    void updateBrightness(int percentage) {
        // byte maxChangePerSecond = 2;
        byte threshold = 5;

        if (lightSensorActive) {
            // Note: setBrightness will only work for any LEDs set AFTER the call.
            // High percentage = a lot of light, so max brightness
            int current = FastLED.getBrightness();
            int value = (getDimFactor() / 100.0) * ((percentage * 255) / 100.0);
            if (value < MIN_BRIGHTNESS) value = MIN_BRIGHTNESS;
            int delta = abs(current - value);
            if (delta < threshold) return;
            //  if (delta > maxChangePerSecond) {
            //     value = current > value ? current - maxChangePerSecond : current + maxChangePerSecond;
            //  }
            FastLED.setBrightness(value);
        }
    }
};

#endif
