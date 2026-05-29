
#ifndef SPARKLINGS_h
#define SPARKLINGS_h

#include "word_clock.h"

using namespace fl;  // Use the FastLED namespace for convenience

class NewYearSparkling {
   private:
    int _duration;
    LedArray *screen = NULL;
    CRGB leds[NUM_LEDS];
    uint32_t nextTrigger = 0;
    XYMap xyMap = XYMap(kMatrixWidth, kMatrixHeight, true);    // For the actual LED output (may be serpentine)
    XYMap xyRect = XYMap(kMatrixWidth, kMatrixHeight, false);  // For the wave simulation (always rectangular grid)
    Blend2d fxBlend = Blend2d(xyMap);                          // Create a blender that will combine the two wave layers
    WaveFx waveFxLower = WaveFx(xyRect, CreateArgsLower());    // Lower/background wave layer (blue)
    WaveFx waveFxUpper = WaveFx(xyRect, CreateArgsUpper());    // Upper/foreground wave layer (green/red)

   public:
    NewYearSparkling(LedArray *clock, int duration = 15000) {
        _duration = duration;
        screen = clock;

        // The order matters - lower layer is added first (background)
        fxBlend.add(waveFxLower);
        fxBlend.add(waveFxUpper);

        waveFxLower.setSpeed(0.14);                                        // Wave propagation speed
        waveFxLower.setDampening(8.2);                                     // How quickly waves lose energy
        waveFxLower.setHalfDuplex(true);                                   // Whether waves can go negative
        waveFxLower.setSuperSample(SuperSample::SUPER_SAMPLE_2X);          // Anti-aliasing quality
        waveFxLower.setEasingMode(U8EasingFunction::WAVE_U8_MODE_LINEAR);  // Wave height calculation method
        waveFxLower.setXCylindrical(false);                                // Set whether lower wave wraps around x-axis

        // Apply all settings from UI controls to the upper wave layer
        waveFxUpper.setSpeed(0.0583);                                      // Wave propagation speed
        waveFxUpper.setDampening(8.1);                                     // How quickly waves lose energy
        waveFxUpper.setHalfDuplex(true);                                   // Whether waves can go negative
        waveFxUpper.setSuperSample(SuperSample::SUPER_SAMPLE_2X);          // Anti-aliasing quality
        waveFxUpper.setEasingMode(U8EasingFunction::WAVE_U8_MODE_LINEAR);  // Wave height calculation method

        Blend2dParams lower_params = {.blur_amount = 0, .blur_passes = 2};
        Blend2dParams upper_params = {.blur_amount = 88, .blur_passes = 5};

        fxBlend.setGlobalBlurAmount(8);
        fxBlend.setGlobalBlurPasses(9);
        fxBlend.setParams(waveFxLower, lower_params);
        fxBlend.setParams(waveFxUpper, upper_params);
    }

    void show() {
        time_t stopAt = millis() + _duration;
        FastLED.setBrightness(255);

        do {
            drawFireworks();
            screen->copyFrom(leds);
            FastLED.show();
        } while (millis() < stopAt);
    }

   private:
    void triggerRipple();
    void applyFancyEffect(uint32_t now);
    void processAutoTrigger(uint32_t now);

    void drawFireworks() {
        uint32_t now = millis();
        applyFancyEffect(now);
        processAutoTrigger(now);
        Fx::DrawContext ctx(now, leds);
        fxBlend.draw(ctx);
    }

    WaveFx::Args CreateArgsLower();
    WaveFx::Args CreateArgsUpper();
};

#endif