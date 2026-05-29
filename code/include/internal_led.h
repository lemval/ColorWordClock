#ifndef INTERNAL_LED_h
#define INTERNAL_LED_h

#include <FastLED.h>

static CRGB lastColor = CRGB(0, 0, 0);

class InternalLed {
 private:
  CRGB leds[1];
  int fadeDelay;
  int blinkDuration;
  int blinkIntensity;
  int lastIntensity;

 public:
  InternalLed(int delay = 25, int duration = 150, int intensity = 15);
  void setFadeDelay(int delay);
  void setBlinkDuration(int duration);
  CRGB getColor();

  void blinkColor(unsigned int color, int times = 1);
  void changeColor(int red, int green, int blue);
  int changeBrightness(int brightness);

  void turnOn();
  void turnOff();

 private:
  void fadeTo(int wanted, unsigned int color);
};

#endif
