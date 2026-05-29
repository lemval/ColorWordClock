#ifndef CONFIG_SERVER_h
#define CONFIG_SERVER_h

#include <Arduino.h>
#include <Preferences.h>
#include <Timezone.h>
#include <WString.h>
#include <WebServer.h>

#include "settings.h"
#include "word_clock.h"

#define SERVER_NAMESPACE "ColorClock"
#define KEY_SERVER "mqttServer"
#define KEY_PORT "mqttPort"
#define KEY_USER "mqttUser"
#define KEY_PASS "mqttPassword"
#define KEY_DEVICE "deviceName"

class ConfigServer {
   private:
    Preferences prefs;
    boolean uptodate = true;
    WebServer webServer;
    time_t startTime = 0;
    Timezone tz = Timezone(getDaylightSaving(), getStandard());

    int mqttPort;
    String mqttServer;
    String mqttUser;
    String mqttPassword;
    String deviceName;

    WordClock* clockRef = NULL;

   public:
    ConfigServer(WordClock* clock);

    int getMqttPort() {
        if (mqttPort == 0) return 1883;
        return mqttPort;
    }

    const char* getDeviceName() { return deviceName.c_str(); }
    const char* getMqttServer() { return mqttServer.c_str(); }
    const char* getMqttUser() { return mqttUser.c_str(); }
    const char* getMqttPassword() { return mqttPassword.c_str(); }
    const Timezone getTimeZone() { return tz; }
    bool hasMqttServer() { return mqttServer != NULL && mqttServer.length() > 0; }

    void setDeviceName(String _value) {
        deviceName = _value;
        uptodate = false;
    }

    void setServer(String _host, int _port) {
        mqttServer = _host;
        mqttPort = _port;
        uptodate = false;
    }

    void setCredentials(String _value1, String _value2) {
        mqttUser = _value1;
        mqttPassword = _value2;
        uptodate = false;
    }

    void setStartTime(long value) {
        if (startTime <= 0) startTime = value;
    }

    void save() {
        if (uptodate) return;
        if (prefs.begin(SERVER_NAMESPACE, false)) {
            if (mqttServer != NULL && mqttServer.length() > 0)
                prefs.putString(KEY_SERVER, mqttServer);
            else
                prefs.remove(KEY_SERVER);

            if (mqttPort > 0)
                prefs.putInt(KEY_PORT, mqttPort);
            else
                prefs.remove(KEY_PORT);

            if (mqttUser != NULL && mqttUser.length() > 0)
                prefs.putString(KEY_USER, mqttUser);
            else
                prefs.remove(KEY_USER);

            if (mqttPassword != NULL && mqttPassword.length() > 0)
                prefs.putString(KEY_PASS, mqttPassword);
            else
                prefs.remove(KEY_PASS);

            if (deviceName != NULL && deviceName.length() > 0)
                prefs.putString(KEY_DEVICE, deviceName);
            else
                prefs.remove(KEY_DEVICE);

            uptodate = true;
            prefs.end();
        } else
            Serial.println("Could not save: system prefs not opened.");
    }

    void handleRequests() { webServer.handleClient(); }

    void startServer();

   private:
    void onRoot();
    void onSave();
    void onConfig();
    void onNotFound();
    void restart();
    String getRootContent();
    String getArgument(String key, String ifNotSet);
    CRGB getArgument(String key, CRGB ifNotSet);
    int getArgument(String key, int ifNotSet);
    float getArgument(String key, float ifNotSet);

    String getParameterContent(const char* key, const char* name, String value, String fieldType);
    String getParameterCheckContent(const char* key, const char* name, bool value);
    String getParameterColorContent(const char* key, const char* name, CRGB value);
    String getParameterNumberContent(const char* key, const char* name, float value, float min, float max, float step);
    String getParameterSelectContent(const char* key, const char* name, int value, String optionList);

    String getStyling();
};

#endif
