#ifndef BOARD_LED_h
#define BOARD_LED_h

#include <WString.h>

class BoardLed {
   private:
    int red;
    int green;
    int blue;

   public:
    BoardLed();
    void activate(int red, int green, int blue);
    void activate(String hexColor);
    void turnOn();
    void turnOff();

    int getRed() { return red; }
    int getGreen() { return green; }
    int getBlue() { return blue; }
};

#endif
