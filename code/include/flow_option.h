#ifndef FLOW_OPTION_h
#define FLOW_OPTION_h

#include <Arduino.h>

class FlowOption {
    int pick;
    int randomValue;

   public:
    FlowOption(const int value) {
        if (value >= 0 && value < SIZE)
            pick = value;
        else
            pick = None;
    }

    int getValue() {
        if (pick == Random) {
            if (randomValue <= Random) deRandomize();
            return randomValue;
        }
        return pick;
    }

    void deRandomize() {
        if (pick == Random) randomValue = random(3, SIZE);
    }

    int next() { return (pick + 1) % SIZE; }

    static const char* getAsString() { return "None;Random;Animate;Flow down;Fade;Swipe;Scanner;Spiral;Fire;Rings;Wave;Mill"; }

    // Special, not selectable
    static const int BirthDay = -1;
    // Options to choose from
    static const int None = 0;
    static const int Random = 1;
    static const int Animate = 2;
    static const int FlowDown = 3;
    static const int Fade = 4;
    static const int Swipe = 5;
    static const int Scanner = 6;
    static const int Spiral = 7;
    static const int Fire = 8;
    static const int Rings = 9;
    static const int Wave = 10;
    static const int Mill = 11;

   private:
    static const int SIZE = 12;
};

#endif
