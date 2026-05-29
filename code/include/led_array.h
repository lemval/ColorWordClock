#ifndef LED_ARRAY_h
#define LED_ARRAY_h

#include <FastLED.h>
#include <Timezone.h>
#include <fx/fx_engine.h>

#include "segments.h"

/*
– Priority 3 (P3) : GPIO pins should be used with caution, as they have one of the following
  important functions, as described in Section 2.3.4 Restrictions for GPIOs and LP GPIOs:
* GPIO4, GPIO5, GPIO8, GPIO9, GPIO15 : Strapping pins.
* GPIO12, GPIO13 : USB Serial/JTAG interface.
* GPIO4, GPIO5, GPIO6, GPIO7 : JTAG interface.
* GPIO16, GPIO17 : UART0 interface.
* GPIO27 : The VDD_SPI pin. The power supply pin for off-package flash by default, and can
  only be reconfigured as a GPIO pin if the flash is powered by an external power supply.

– Priority 4 (P4) : GPIO pins already allocated or not recommended for use, as described in
  Section 2.3.4 Restrictions for GPIOs and LP GPIOs:
* GPIO24, GPIO25, GPIO26, GPIO28, GPIO29, GPIO30 : SPI0/1 interface recommended for the off-package flash*/
#define DATA_PIN 20
#define CLOCK_PIN 19

class LedArray {
    CRGB ledData[NUM_LEDS];

   public:
    LedArray(boolean active = false) {
        clear();
        if (active) FastLED.addLeds<HD107HD, DATA_PIN, CLOCK_PIN, BGR>(ledData, NUM_LEDS);
    }

    LedArray(LedArray* copy) { copyFrom(copy); }

    CRGB get(int pos) { return ledData[pos]; }
    void set(int pos, CRGB color) { ledData[pos] = color; }

    void clear() {
        for (int i = 0; i < NUM_LEDS; i++) ledData[i] = CRGB::Black;
    }

    void clearClockFace() {
        for (int i = 0; i < FACE_LEDS; i++) ledData[i] = CRGB::Black;
    }

    void copyFrom(LedArray* data) {
        for (int i = 0; i < NUM_LEDS; i++) ledData[i] = data->ledData[i];
    }

    void copyFrom(CRGB* data) {
        for (int i = 0; i < NUM_LEDS; i++) ledData[i] = data[i];
    }

    void copyFrom(CHSV* data) {
        for (int i = 0; i < NUM_LEDS; i++) ledData[i] = data[i];
    }

    void render(fl::FxEngine* fxEngine) { fxEngine->draw(millis(), ledData); }

    void fade(int flowTime = 0) {
        for (size_t i = 0; i < 50; i++) {
            fadeToBlackBy(ledData, FACE_LEDS, 6);
            FastLED.show();
            delay(flowTime / 100);
        }
    }

    void blendTo(LedArray* target, int flowTime = 0) {
        LedArray source = LedArray(this);
        for (size_t step = 5; step <= 255; step += 5) {
            for (int i = 0; i < FACE_LEDS; i++) {
                ledData[i] = fl::nblend(source.ledData[i], target->ledData[i], step);
            }
            FastLED.show();
            delay(flowTime / 50);
        }
    }
};

#endif
