#include "board_led.h"

#include <Arduino.h>

BoardLed::BoardLed() {
  red = 0;
  green = 0;
  blue = 0;
}

void BoardLed::turnOff() { rgbLedWrite(RGB_BUILTIN, 0, 0, 0); }

void BoardLed::turnOn() {
  if (red == 0 && green == 0 && blue == 0) {
    red = 255;
    green = 255;
    blue = 255;
  }
  rgbLedWrite(RGB_BUILTIN, red, green, blue);
}

void BoardLed::activate(int _red, int _green, int _blue) {
  red = max(0, min(255, _red));
  green = max(0, min(255, _green));
  blue = max(0, min(255, _blue));

  turnOn();
}

void BoardLed::activate(String hexColor) {
  int size = 0;
  String color = "000000";
  String alpha = "FF";

  if (hexColor != NULL) size = sizeof(hexColor);
  if (size > 0) {
    int start = 0;
    if (hexColor[0] == '#') start = start + 1;

    if (size >= 6 + start) color = hexColor.substring(start, start + 6);
    if (size >= 8 + start) alpha = hexColor.substring(start + 6, start + 8);
  }

  long colorInt = strtol(color.c_str(), NULL, 16);
  long alphaInt = strtol(alpha.c_str(), NULL, 16);  // Ignore for now

  red = (colorInt & 0xFF0000) >> 16;
  green = (colorInt & 0x00FF00) >> 8;
  blue = colorInt & 0x0000FF;

  turnOn();
}
