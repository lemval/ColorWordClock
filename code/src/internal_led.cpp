#include "internal_led.h"

InternalLed::InternalLed(int delay, int duration, int intensity) {
    fadeDelay = delay;
    blinkDuration = duration;
    blinkIntensity = max(10, intensity);
    // Internal led is on pin 8
    FastLED.addLeds<NEOPIXEL, 8>(leds, 1);
}

CRGB InternalLed::getColor() { return leds[0]; }

void InternalLed::turnOn() {
    // Intensity is included in the color value...
    int level = max(min(255, lastIntensity), blinkIntensity);
    if (level != FastLED.getBrightness()) {
        fadeTo(level, lastColor.as_uint32_t());
    }
    FastLED.show();
}

void InternalLed::turnOff() {
    lastIntensity = leds[0].getAverageLight();
    lastColor = leds[0];
    fadeTo(0, leds[0].as_uint32_t());
}

void InternalLed::setFadeDelay(int delay) {
    if (delay >= 0 && delay <= 250) fadeDelay = delay;
}

void InternalLed::setBlinkDuration(int duration) {
    if (duration >= 0 && duration <= 5000) blinkDuration = duration;
}

void InternalLed::changeColor(int red, int green, int blue) {
    int level = max(blinkIntensity, (int)FastLED.getBrightness());
    uint32_t color = CRGB(red, green, blue).as_uint32_t();
    fadeTo(level, color);
}

int InternalLed::changeBrightness(int brightness) {
    int level = max(min(255, brightness), 0);
    fadeTo(brightness, leds[0].as_uint32_t());
    return FastLED.getBrightness();
}

void InternalLed::fadeTo(int wanted, unsigned int color) {
    int current = FastLED.getBrightness();
    int realColor = color;
    if (realColor <= 0) realColor = leds[0].as_uint32_t();

    if (wanted > current) {
        for (int i = current; i <= wanted; i++) {
            leds[0] = color;
            FastLED.setBrightness(i);
            FastLED.show();
            delay(fadeDelay);
        }
    } else if (wanted < current) {
        for (int i = current; i >= wanted; i--) {
            leds[0] = color;
            FastLED.setBrightness(i);
            FastLED.show();
            delay(fadeDelay);
        }
    } else {
        leds[0] = color;
        FastLED.show();
        delay(fadeDelay);
    }
    lastColor = CRGB(color);
}

void InternalLed::blinkColor(unsigned int color, int times) {
    for (int i = 0; i < times; i++) {
        fadeTo(blinkIntensity, color);
        delay(blinkDuration);
        fadeTo(0, color);
        delay(blinkDuration);
    }
}
