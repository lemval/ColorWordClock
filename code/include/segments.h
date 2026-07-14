#ifndef SEGMENTS_h
#define SEGMENTS_h

#include <cstring>

#include "settings.h"

#define NUM_LEDS 173
#define FACE_LEDS 156
#define SIGN_LEDS 13
#define kMatrixWidth 13
#define kMatrixHeight 13
#define MAX_SEGMENT_SIZE 15

class Segment {
   public:
    int size;
    int data[MAX_SEGMENT_SIZE];

    Segment(int elem, ...) {
        va_list arguments;
        va_start(arguments, elem);
        size = elem;
        for (int x = 0; x < size && x < MAX_SEGMENT_SIZE; x++) {
            data[x] = va_arg(arguments, int);
        }
        va_end(arguments);
    }
};

// Note: Wiring is done reversed on every second line (serpentine)

#ifdef NL_CLOCK_FACE

// ! X OBM K P Y WXD Q M M B D L Y
// 3, 6, 10, 11, 12, 17, 38, 39, 45, 46, 47, 72, 86, 94, 99, 108, 117, 146

const Segment denotion(5, 0, 1, 2, 4, 5);                      // HET IS
const Segment minOne(3, 7, 8, 9);                              // EEN
const Segment minTwo(4, 22, 23, 24, 25);                       // TWEE
const Segment minThree(4, 18, 19, 20, 21);                     // DRIE
const Segment minFour(4, 13, 14, 15, 16);                      // VIER
const Segment minFive(4, 26, 27, 28, 29);                      // VIJF
const Segment minSix(3, 30, 31, 32);                           // ZES
const Segment minSeven(5, 33, 34, 35, 36, 37);                 // ZEVEN
const Segment minEight(4, 48, 49, 50, 51);                     // ACHT
const Segment minNine(5, 40, 41, 42, 43, 44);                  // NEGEN
const Segment minTen(4, 52, 53, 54, 55);                       // TIEN
const Segment minEleven(3, 56, 57, 58);                        // ELF
const Segment minTwelve(6, 59, 60, 61, 62, 63, 64);            // TWAALF
const Segment minThirteen(7, 65, 66, 67, 68, 69, 70, 71);      // DERTIEN
const Segment minFourteen(8, 78, 79, 80, 81, 82, 83, 84, 85);  // VEERTIEN
const Segment quarter(5, 73, 74, 75, 76, 77);                  // KWART
const Segment before(4, 87, 88, 89, 90);                       // VOOR
const Segment over(4, 100, 101, 102, 103);                     // OVER
const Segment half(4, 95, 96, 97, 98);                         // HALF
const Segment hrFull(3, 143, 144, 145);                        // UUR
const Segment hrOne(3, 91, 92, 93);                            // EEN
const Segment hrTwo(4, 104, 105, 106, 107);                    // TWEE
const Segment hrThree(4, 109, 110, 111, 112);                  // DRIE
const Segment hrFour(4, 113, 114, 115, 116);                   // VIER
const Segment hrFive(4, 126, 127, 128, 129);                   // VIJF
const Segment hrSix(3, 123, 124, 125);                         // ZES
const Segment hrSeven(5, 118, 119, 120, 121, 122);             // ZEVEN
const Segment hrEight(4, 130, 131, 132, 133);                  // ACHT
const Segment hrNine(5, 134, 135, 136, 137, 138);              // NEGEN
const Segment hrTen(4, 139, 140, 141, 142);                    // TIEN
const Segment hrEleven(3, 153, 154, 155);                      // ELF
const Segment hrTwelve(6, 147, 148, 149, 150, 151, 152);       // TWAALF

const int skipSignLeds[] = {};
#endif

#ifdef EN_CLOCK_FACE

// A QUARTER TO / PAST SIX / SIX-THIRTY
// ! X B L A M L B Z D - LEM X
// 2, 5, 65, 82, 87, 90, 100, 96, 108, 113, 140, 141, 142, 149

const Segment denotion(4, 0, 1, 3, 4);                         // IT IS
const Segment minOne(3, 6, 7, 8);                              // ONE
const Segment minTwo(3, 10, 11, 12);                           // TWO
const Segment minThree(5, 21, 22, 23, 24, 25);                 // THREE
const Segment minFour(4, 17, 18, 19, 20);                      // FOUR
const Segment minFive(4, 13, 14, 15, 16);                      // FIVE
const Segment minSix(3, 26, 27, 28);                           // SIX
const Segment minSeven(5, 29, 30, 31, 32, 33);                 // SEVEN
const Segment minEight(5, 34, 35, 36, 37, 38);                 // EIGHT
const Segment minNine(4, 48, 49, 50, 51);                      // NINE
const Segment minTen(3, 45, 46, 47);                           // TEN
const Segment minEleven(6, 39, 40, 41, 42, 43, 44);            // ELEVEN
const Segment minTwelve(6, 52, 53, 54, 55, 56, 57);            // TWELVE
const Segment quarter(8, 9, 58, 59, 60, 61, 62, 63, 64);       // A QUARTER
const Segment minThirteen(8, 77, 76, 75, 74, 69, 68, 67, 66);  // THIRTEEN
const Segment minFourteen(8, 73, 72, 71, 70, 69, 68, 67, 66);  // FOURTIEN
const Segment before(2, 88, 89);                               // TO
const Segment over(4, 83, 84, 85, 86);                         // PAST
const Segment half(6, 148, 147, 146, 145, 144, 143);           // THIRTY
const Segment hrOne(3, 103, 102, 101);                         // ONE
const Segment hrTwo(3, 99, 98, 97);                            // TWO
const Segment hrThree(5, 95, 94, 93, 92, 91);                  // THREE
const Segment hrFour(4, 104, 105, 106, 107);                   // FOUR
const Segment hrFive(4, 109, 110, 111, 112);                   // FIVE
const Segment hrSix(3, 114, 115, 116);                         // SIX
const Segment hrSeven(5, 129, 128, 127, 126, 125);             // SEVEN
const Segment hrEight(5, 124, 123, 122, 121, 120);             // EIGHT
const Segment hrTen(3, 119, 118, 117);                         // TEN
const Segment hrNine(4, 130, 131, 132, 133);                   // NINE
const Segment hrEleven(6, 134, 135, 136, 137, 138, 139);       // ELVEN
const Segment hrTwelve(6, 155, 154, 153, 152, 151, 150);       // TWELVE
const Segment hrHalf(4, 78, 79, 80, 81);                       // HALF
const Segment hrFull(6, 159, 161, 162, 163, 164, 165);         // O CLOCK

// Positions in the skip array
const int skipSignLeds[] = {3, 4, 5, 6, 7, 8, 9};

#endif

// English clock does not have 159, 160, 161, 162, 163, 164, 165 as sign bar leds
const Segment signBar(SIGN_LEDS, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168);

const Segment dots(4, 172, 169, 170, 171);

const Segment minuteBlock[15] = {minOne,  minTwo, minThree,  minFour,   minFive,     minSix,      minSeven, minEight,
                                 minNine, minTen, minEleven, minTwelve, minThirteen, minFourteen, quarter};
const Segment hourBlock[12] = {hrTwelve, hrOne, hrTwo, hrThree, hrFour, hrFive, hrSix, hrSeven, hrEight, hrNine, hrTen, hrEleven};

const uint8_t XYTable[] = {0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,   //
                           25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,  13,   //
                           26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,   //
                           51,  50,  49,  48,  47,  46,  45,  44,  43,  42,  41,  40,  39,   //
                           52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,   //
                           77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,   //
                           78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,   //
                           103, 102, 101, 100, 99,  98,  97,  96,  95,  94,  93,  92,  91,   //
                           104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,  //
                           129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117,  //
                           130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,  //
                           155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, 143,  //
                           156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168};

/**
 * Only calculates the active digits!
 */
class Segments {
   private:
    bool clockFace[NUM_LEDS];
    int clockFaceHourMinute = -1;
    int16_t size = sizeof(clockFace);
    int16_t blinkPos = 162;  // First blink the signal bar...

    bool isValid(int position) {
        for (int i : skipSignLeds)
            if (position == i) return false;
        return true;
    }

   public:
    bool isActive(int digit) { return clockFace[digit]; }
    int getBlinkSpot() { return blinkPos; }
    int getSignSpot(int pos) { return (pos < SIGN_LEDS && pos >= 0 && isValid(pos)) ? signBar.data[pos] : -1; }

    static uint8_t XY(uint8_t x, uint8_t y) { return XYTable[(y * kMatrixWidth) + x]; };

    int getActiveDigits() {
        int active = 0;
        for (int i = 0; i < FACE_LEDS; i++)
            if (clockFace[i]) active++;
        return active;
    }

    void countDown(int secondsToGo) {
        memset(clockFace, false, size * sizeof(bool));
        updateFace(minuteBlock[max(0, min(secondsToGo - 1, 14))]);
    }

    bool calculate(int hour, int minute, int second) {
        int value = hour * 100 + minute;
        int updated = false;

        if (value != clockFaceHourMinute) {
            clockFaceHourMinute = value;
            updated = true;

            int updatedHour = hour;

            // Clear clockface, then set all applicable
            memset(clockFace, false, size * sizeof(bool));

            // Calculate new view
            updateFace(denotion);

            if (minute == 0) {
                updateFace(hrFull);
            } else if (minute > 0 && minute <= 15) {
                updateFace(minuteBlock[minute - 1]);
                updateFace(over);
            } else if (minute > 15 && minute < 30) {
                updateFace(minuteBlock[29 - minute]);
                updateFace(before);
                updateFace(half);
#ifdef NL_CLOCK_FACE
                updatedHour += 1;
#endif
            } else if (minute == 30) {
#ifdef NL_CLOCK_FACE
                updateFace(half);
                updatedHour += 1;
#endif
#ifdef EN_CLOCK_FACE
                updateFace(hrHalf);
                updateFace(over);
#endif
            } else if (minute > 30 && minute < 45) {
                updateFace(minuteBlock[minute - 31]);
                updateFace(over);
                updateFace(half);
#ifdef NL_CLOCK_FACE
                updatedHour += 1;
#endif
            } else if (minute >= 45 && minute < 60) {
                updateFace(minuteBlock[59 - minute]);
                updateFace(before);
                updatedHour += 1;
            }

            // TEMP
            updateFace(hrFull);

            updateFace(hourBlock[updatedHour % 12]);
        }

        // For flashing seconds
        blinkPos = dots.data[second / 15];

        return updated;
    }

   private:
    void updateFace(Segment segment) {
        for (int i = 0; i < segment.size; i++) {
            int index = segment.data[i];
            clockFace[index] = true;
        }
    }
};

#endif
