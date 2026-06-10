#ifndef TICK_OPTION_h
#define TICK_OPTION_h

class TickOption {
    int pick;

   public:
    TickOption(const int value) {
        if (value >= 0 && value < SIZE)
            pick = value;
        else
            pick = Blink;
    }

    int getValue() { return pick; }
    int next() { return (pick + 1) % SIZE; }

    static const char* getAsString() { return "Off;Blink;Solid;Fade"; }
    static const int Off = 0;
    static const int Blink = 1;
    static const int Solid = 2;
    static const int Fade = 3;

   private:
    static const int SIZE = 4;
};

#endif
