#ifndef ANALOG_READER_h
#define ANALOG_READER_h

// Sensor used: 5516
// Inverted = 3.3V - 4K7 - SENSOR - LDR - GND
// Regular = 3.3V - LDR  - SENSOR - 4K7 - GND

#define NUM_SENSORS 1
#define PIN_SENSOR 6

#include <Arduino.h>

#include "settings.h"

class AnalogReader {
    class Sensor {
        const char* name;

       public:
        int16_t value;  // 12 bits
        bool inverted;
        int pin;

        Sensor(const char* _name, int _pin, bool _inverted = false) {
            pin = _pin;
            name = _name;
            inverted = _inverted;
            pinMode(pin, INPUT_PULLUP);
        }

        const char* getName() { return name; }
    };

    // TODO Make sure to give the correct value to 'inverted'

    // Only use ADC1 (ADC2 is used for WiFi)
    Sensor sensors[NUM_SENSORS] = {Sensor("Light Intensity", PIN_SENSOR, INVERTED)};

   public:
    int lowestValue;
    int highestValue;

   public:
    AnalogReader() {
        // 0: 100-950 mV, 2_5: 100-1250 mV, 6: 150-1750 mV, 11: 150 - 3100 mV
        // See also https://circuitlabs.net/adc-configuration-and-reading-with-esp32/
        analogSetAttenuation(ADC_11db);
        lowestValue = 500;
        highestValue = 2500;
    }

    void read() {
        for (size_t i = 0; i < NUM_SENSORS; i++) {
            int16_t val = analogRead(sensors[i].pin);
            // Serial.printf("Read value of %d [%d - %d]\n", val, lowestValue, highestValue);
            if (val > highestValue) highestValue = val;
            if (val < lowestValue) lowestValue = val;
            sensors[i].value = val;
        }
    }

    Sensor getSensor(int sensor = 0) { return sensors[sensor]; }

    int getPercentage(int sensor = 0) {
        int percentage = 100.0 * (sensors[sensor].value - lowestValue) / (highestValue - lowestValue);
        return (sensors[sensor].inverted) ? 100 - percentage : percentage;
    }
};

#endif
