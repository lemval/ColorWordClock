#include "config_server.h"

#include <TimeLib.h>

#include "HTTP_Method.h"
#include "ha_settings.h"

#define VALUE_HIDDEN "********* "

#define S_NAME "name"
#define S_SERVER "server"
#define S_PORT "port"
#define S_USER "user"
#define S_PASS "pass"
#define CFG_FLOW "cfg_clock_flow"
#define CFG_FACE "cfg_clock_face"
#define CFG_STAT "cfg_clock_stats"
#define CFG_TIME "cfg_flow_time"
#define CFG_SPEED "cfg_flow_speed"
#define CFG_PATTERN "cfg_flow_pattern"
#define CFG_LIGHT "cfg_light"
#define CFG_TICK "cfg_tick"
#define CFG_COLOR "cfg_clock_color"
#define CFG_TKCLR "cfg_tick_color"
#define CFG_DIM "cfg_dim"

const char compile_date[] = __DATE__ " " __TIME__;

ConfigServer::ConfigServer(WordClock* clock) { clockRef = clock; }

void ConfigServer::startServer() {
    // Open in readonly mode
    if (prefs.begin(SERVER_NAMESPACE, true)) {
        if (prefs.isKey(KEY_SERVER)) mqttServer = prefs.getString(KEY_SERVER);
        if (prefs.isKey(KEY_PORT)) mqttPort = prefs.getInt(KEY_PORT);
        if (prefs.isKey(KEY_USER)) mqttUser = prefs.getString(KEY_USER);
        if (prefs.isKey(KEY_PASS)) mqttPassword = prefs.getString(KEY_PASS);
        if (prefs.isKey(KEY_DEVICE)) deviceName = prefs.getString(KEY_DEVICE);
    } else {
        // Write defaults
        mqttServer = "";
        mqttPort = 1883;
        deviceName = SERVER_NAMESPACE;
        save();
    }
    prefs.end();

    if (mqttServer != NULL) mqttServer.trim();
    if (mqttUser != NULL) mqttUser.trim();

    webServer.on("/", HTTP_GET, std::bind(&ConfigServer::onRoot, this));
    webServer.on("/", HTTP_POST, std::bind(&ConfigServer::onSave, this));
    webServer.on("/settings", HTTP_POST, std::bind(&ConfigServer::onConfig, this));
    webServer.on("/restart", HTTP_POST, std::bind(&ConfigServer::restart, this));
    webServer.onNotFound(std::bind(&ConfigServer::onNotFound, this));
    webServer.begin();
}

void ConfigServer::onRoot() { webServer.send(200, "text/html", getRootContent()); }
void ConfigServer::onNotFound() { webServer.send(404, "text/plain", "Resource not found!"); }

void ConfigServer::onSave() {
    setDeviceName(getArgument(S_NAME, deviceName));
    setServer(getArgument(S_SERVER, mqttServer), getArgument(S_PORT, 1883));
    setCredentials(getArgument(S_USER, mqttUser), getArgument(S_PASS, mqttPassword));
    save();
    Serial.println("Settings saved.");
    onRoot();
}

void ConfigServer::onConfig() {
    clockRef->setAnimation(getArgument(CFG_PATTERN, -1));
    clockRef->setAnimationSpeed(getArgument(CFG_SPEED, 1.0f));
    clockRef->setFlowTime(getArgument(CFG_TIME, 1500));
    clockRef->setTickPattern(getArgument(CFG_TICK, clockRef->getTickOption()));
    clockRef->setLightSensorState(getArgument(CFG_LIGHT, "off").equals("on"));
    clockRef->setFlowPattern(getArgument(CFG_FLOW, clockRef->getFlowOption()));
    clockRef->setFacePattern(getArgument(CFG_FACE, clockRef->getFaceOption()));
    clockRef->setStatusPattern(getArgument(CFG_STAT, clockRef->getStatusOption()));
    CRGB color = getArgument(CFG_TKCLR, clockRef->getTickColor());
    clockRef->setTickColor(color.r, color.g, color.b);
    clockRef->setColor(getArgument(CFG_COLOR, clockRef->getCustomColor()));
    clockRef->setDimFactor(getArgument(CFG_DIM, clockRef->getDimFactor()));
    Serial.println("Config saved.");
    onRoot();
}

void ConfigServer::restart() {
    Serial.println("Restarting on command.");
    webServer.sendHeader("Location", "/", true);
    webServer.send(302, "text/plain", "Moved");
    ESP.restart();
}

String ConfigServer::getRootContent() {
    String title = deviceName + " Config Server";
    char start[20];
    sprintf(start, "%02d.%02d.%02d %02d:%02d", year(startTime), month(startTime), day(startTime), hour(startTime), minute(startTime));
    String connected = getHomeAssistantStatus();
    String result = "<!DOCTYPE html><html><head><title>" + title + "</title>" + getStyling() +  //
                    "</head><body><h1>" + title + "</h1>" +                                     //
                    "<small><p>Built on: " + compile_date + "<br/>Running since: " + start + "<br/>MQTT connected: " + connected +
                    "<br/></p></small>" + "<h2>Server settings</h2>";

    return result + "<form action=\"/\" method=\"POST\">" +                  //
           getParameterContent(S_NAME, "Device name", deviceName, "text") +  //
           getParameterContent(S_SERVER, "MQTT server", mqttServer, "text") +
           getParameterContent(S_PORT, "MQTT port (default: 1883)", String(mqttPort), "number") +
           getParameterContent(S_USER, "MQTT user (leave empty if none)", mqttUser, "text") +
           getParameterContent(S_PASS, "MQTT password", VALUE_HIDDEN, "password") +

           "<div><input type=\"submit\" value=\"Save!\" onClick=\"this.disabled=true; this.value='Saving...'; this.form.submit();\" "
           "/></div>" +

           "</form><form action=\"/restart\" method=\"POST\">" +  //
           "<div><input style=\"background: #f89c9c; position:inherit;\" type=\"submit\" value=\"Reboot\" onClick "
           "=\"this.disabled=true; this.value='Rebooting...'; this.form.submit();\" /></div>" +
           "</form><p>Save first, then reboot!</p>" + "<h2>Clock settings</h2>" +  //
           "<form action=\"/settings\" method=\"POST\">" +

           getParameterNumberContent(CFG_TIME, "Flow time", clockRef->getFlowTime(), 0, 5000, 100) +
           getParameterNumberContent(CFG_SPEED, "Flow speed", clockRef->getAnimationSpeed(), 0.1f, 2.5f, 0.1f) +
           getParameterNumberContent(CFG_PATTERN, "Flow pattern", clockRef->getAnimation(), -1, 50, 1) +

           getParameterCheckContent(CFG_LIGHT, "Light sensor", clockRef->getLightSensorState()) +
           getParameterSelectContent(CFG_TICK, "Seconds blinker", clockRef->getTickOption(), TickOption::getAsString()) +

           getParameterSelectContent(CFG_FLOW, "Minute transition", clockRef->getFlowOption(), FlowOption::getAsString()) +
           getParameterSelectContent(CFG_FACE, "Clock face", clockRef->getFaceOption(), FaceOption::getAsString()) +
           getParameterSelectContent(CFG_STAT, "Status field", clockRef->getStatusOption(), StatusOption::getAsString()) +

           getParameterColorContent(CFG_COLOR, "Custom clock color", clockRef->getCustomColor()) +
           getParameterColorContent(CFG_TKCLR, "Seconds color", clockRef->getTickColor()) +

           getParameterNumberContent(CFG_DIM, "Brightness correction", clockRef->getDimFactor(), 0, 100, 1) +

           "<div><input type=\"submit\" value=\"Configure!\" onClick=\"this.disabled=true; this.value='Updating...'; this.form.submit();\"/> "
           "</div> " +

           "</body></html>";
}

String ConfigServer::getArgument(String key, String ifNotSet) {
    if (!webServer.hasArg(key)) return ifNotSet;
    String value = webServer.arg(key);
    if (value.equals(VALUE_HIDDEN)) return ifNotSet;
    return value;
}

CRGB ConfigServer::getArgument(String key, CRGB ifNotSet) {
    if (!webServer.hasArg(key)) return ifNotSet;
    String value = webServer.arg(key);
    u32_t number = strtol(value.substring(1).c_str(), nullptr, 16);
    return CRGB(number);
}

int ConfigServer::getArgument(String key, int ifNotSet) {
    if (!webServer.hasArg(key)) return ifNotSet;
    String value = webServer.arg(key);
    int result = value.toInt();
    if (result <= 0) return ifNotSet;
    return result;
}

float ConfigServer::getArgument(String key, float ifNotSet) {
    if (!webServer.hasArg(key)) return ifNotSet;
    String value = webServer.arg(key);
    float result = value.toFloat();
    if (result <= 0) return ifNotSet;
    return result;
}

String ConfigServer::getParameterContent(const char* key, const char* name, String value, String type) {
    String element = String(key);
    String prefill = value;
    if (prefill == NULL) prefill = "";

    return "<div class=\"form-item\"><label for=\"" + element + "\">" + name + ":</label>" +  //
           "<input type=\"" + type + "\" name=\"" + element + "\" id=\"" + element + "\" value=\"" + prefill + "\"/></div>";
}

String ConfigServer::getParameterCheckContent(const char* key, const char* name, bool value) {
    String element = String(key);
    String checked = value ? "checked" : "";
    return "<div class=\"form-item\"><label for=\"" + element + "\">" + name + ":</label>" +  //
           "<input type=\"checkbox\" name=\"" + element + "\" id=\"" + element + "\" value=\"on\" " + checked + "/></div>";
}

String ConfigServer::getParameterColorContent(const char* key, const char* name, CRGB value) {
    String element = String(key);
    char color[8];
    sprintf(color, "#%06x", value.as_uint32_t() & 0xFFFFFF);

    return "<div class=\"form-item\"><label for=\"" + element + "\">" + name + ":</label>" +                    //
           "<input type=\"color\" name=\"" + element + "\" id=\"" + element + "\" value=\"" + color + "\"/>" +  //
           "</div>";
}

String ConfigServer::getParameterNumberContent(const char* key, const char* name, float value, float min, float max, float step) {
    String element = String(key);
    return "<div class=\"form-item\"><label for=\"" + element + "\">" + name + ":</label>" +  //
           "<input type=\"number\" name=\"" + element + "\" id=\"" + element + "\" value=\"" + String(value) + "\" min=\"" + String(min) +
           "\" max=\"" + String(max) + "\" step=\"" + String(step) + "\"/></div>";
}

String ConfigServer::getParameterSelectContent(const char* key, const char* name, int selection, String optionList) {
    String element = String(key);
    String result;
    int index = 0;
    int len = optionList.length() + 1;
    char charArray[len];
    optionList.toCharArray(charArray, len);
    char* token = strtok(charArray, ";");
    while (token != NULL) {
        const char* sel = (selection == index ? " selected" : "");
        char opt[38 + strlen(token)];
        sprintf(opt, "<option value=\"%2d\"%s>%s</option>", index, sel, token);
        token = strtok(NULL, ";");
        result += opt;
        index++;
    }

    return "<div class=\"form-item\"><label for=\"" + element + "\">" + name + ":</label>" +  //
           "<select name=\"" + element + "\" id=\"" + element + "\">" +                       //
           result +                                                                           //
           "</select></div>";
}

String ConfigServer::getStyling() {
    return R"styling(
<style>
    .form-item {
        margin: 10px;
    }

    body {
        font-size: 22px;
    }

    h1,
    h2 {
        width: 100%;
        background: #cfc;
        padding: 2px 10px;
    }

    input,
    select {
        margin-left: 10px;
        left: 325px;
        position: absolute;
        font-size: 17px;
    }

    input[type=submit] {
        padding: 10px 25px;
        font-weight: bold;
        background: #99bbff;
        border: 0 none;
        cursor: pointer;
        -webkit-border-radius: 5px;
        border-radius: 5px;
        margin: 10px;
    }
</style>
    )styling";
}
