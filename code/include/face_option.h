#ifndef FACE_OPTION_h
#define FACE_OPTION_h

#include "settings.h"

class FaceOption {
    int pick;

   public:
    FaceOption(const int value) {
        if (value >= 0 && value < SIZE)
            pick = value;
        else
            pick = Warm;
    }

    int getValue() { return pick; }
    int next() { return (pick + 1) % SIZE; }

    static const char* getAsString() { return TXT_FACE_OPTIONS; }
    static const int Warm = 0;
    static const int Custom = 1;
    static const int Rainbow = 2;
    static const int Animate = 3;
    static const int ColorWord = 4;
    static const int ColorChar = 5;

   private:
    static const int SIZE = 6;
};

#endif
