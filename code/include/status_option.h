#ifndef STATUS_OPTION_h
#define STATUS_OPTION_h

#include <FastLED.h>

#include "settings.h"

class StatusOption {
    int pick;
    int day = 0;
    int month = 0;

   public:
    StatusOption(const int value) { pick = (value >= 0 && value < SIZE) ? value : Default; }

    int getValue() { return pick; }

    bool updateTime(int _day, int _month) {
        if (_day == day && _month == month) return false;
        day = _day;
        month = _month;
        return true;
    }

    CRGB getColorFor(int idx, CRGB tickColor, CRGB clockColor) {
        bool matchAlert = idx > 3 && idx < 9;
        bool useDate = true;
#ifdef EN_CLOCK_FACE
        matchAlert = idx <= 3 || idx >= 9;
        useDate = false;
#endif
        switch (pick) {
            case Default:
                return idx % 2 == 1 ? CRGB(0xFF890E) : CRGB::Black;
            case Tick:
                return tickColor;
            case Clock:
                return clockColor;
            case NoAlert:
                return matchAlert ? CRGB(0x43FF06) : CRGB::Black;
            case MildAlert:
                return matchAlert ? CRGB(0xFFA009) : CRGB::Black;
            case HighAlert:
                return matchAlert ? CRGB(0xFF1900) : CRGB::Black;
            case Date:
                return useDate ? getDateBitColor(idx, tickColor) : CRGB::Black;
            case Individual:
            default:
                return CRGB::Black;
        }
    }

    bool isAnimated() { return pick == MildAlert || pick == HighAlert; }

    static const char* getAsString() { return TXT_STAT_OPTIONS; }
    static const int Default = 0;
    static const int Individual = 1;
    static const int Tick = 2;
    static const int Clock = 3;
    static const int NoAlert = 4;
    static const int MildAlert = 5;
    static const int HighAlert = 6;
    static const int Date = 7;

   private:
    static const int SIZE = 8;

    CRGB getDateBitColor(int idx, CRGB on) {
        CHSV brightnessGiver = rgb2hsv_approximate(on);
        CRGB offColor = CRGB(CHSV(brightnessGiver.hue, 0.9 * brightnessGiver.sat, max(50.0, 0.5 * brightnessGiver.val)));

        switch (idx) {
            case 1 ... 5:
                // Bitwise day; 1 = MSB
                return (day >> (5 - idx)) & 0x01 == 1 ? on : offColor;
            case 9 ... 12:
                // Bitwise month; 1 = MSB
                return (month >> (12 - idx)) & 0x01 == 1 ? on : offColor;
            default:
                return CRGB::Black;
        }
    }
};

#endif
