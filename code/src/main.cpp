#include <Arduino.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>

#include "analog_reader.h"
#include "board_led.h"
#include "config_server.h"
#include "ha_settings.h"
#include "settings.h"
#include "switch_control.h"
#include "wifi_connect.h"
#include "word_clock.h"

NetworkUDP udp;
NTPClient ntp = NTPClient(udp);

WordClock wordClock;
ConfigServer configServer(&wordClock);
AnalogReader analog;
SwitchControl control;

// Should span over threads
volatile bool tick = false;

uint32_t last_ota_time = 0;

hw_timer_t* timer = NULL;

void IRAM_ATTR timerTask() { tick = true; }

void setupOTA() {
    ArduinoOTA.setPort(55910);
    ArduinoOTA.setHostname(WiFi.getHostname());
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart([]() { Serial.println("[Update] started"); })
        .onEnd([]() { Serial.println("[Update] End"); })
        .onError([](ota_error_t error) { Serial.printf("[Update] Error %d while uploading!\n", error); })
        .onProgress([](unsigned int progress, unsigned int total) {
            if (millis() - last_ota_time > 500) {
                Serial.printf("[Update] Progress: %u%%\n", (progress / (total / 100)));
                last_ota_time = millis();
            }
        });
    ArduinoOTA.begin();
}

void setup() {
    setupWifiConfiguredConnection();

    configServer.startServer();

    ntp.setUpdateInterval(50000 * 60 * 5);
    ntp.begin();

    wordClock.initialize();

    setupHomeAssist(&wordClock, &configServer);
    control.setup(&wordClock);

    setupOTA();

    timer = timerBegin(1000000);  //  1Mhz resolution timer
    timerAttachInterrupt(timer, &timerTask);
    timerAlarm(timer, 1000 * 1000, true, 0);  // Each second an alarm

    Serial.println("Clock started!");
}

void updateLightLevel() {
    if (!wordClock.getLightSensorState()) return;

    analog.read();
    int percentage = analog.getPercentage() * 1;  // TODO
    wordClock.updateBrightness(percentage);
    updateAnalog(percentage);
}

void loop() {
    ArduinoOTA.handle();
    control.loop();
    loopMqtt();

    if (tick) {
        tick = false;
        ntp.update();

        if (ntp.isTimeSet()) {
            Timezone tz = configServer.getTimeZone();
            long now = tz.toLocal(ntp.getEpochTime());
            configServer.setStartTime(now);
            wordClock.update(now);
        }

        updateLightLevel();
        updateIP(WiFi.localIP().toString().c_str());
    }

    configServer.handleRequests();
    wordClock.animate();

    // Do not delay here as this is blocking HA to work correctly.
}
