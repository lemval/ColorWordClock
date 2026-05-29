#ifndef HA_SETTINGS_h
#define HA_SETTINGS_h

#include "analog_reader.h"
#include "config_server.h"
#include "word_clock.h"

void setupHomeAssist(WordClock *clock, ConfigServer *cfg);

String getHomeAssistantStatus();

void loopMqtt();
void updateAnalog(int percentage);
void updateIP(const char* address);
void updateSettings(FlowOption flow, FaceOption face, bool tick, bool sensor, int16_t time);

#endif
