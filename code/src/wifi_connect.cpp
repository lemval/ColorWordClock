#include "wifi_connect.h"

// void onWiFiEvent(WiFiEvent_t event);

void setupWifiConfiguredConnection() {
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want
    // it.

    Serial.begin(115200);
    // WiFi.onEvent(onWiFiEvent);
    WiFi.setHostname("ColorClock");

    // WiFiManager, Local intialization. Once its business is done, there is no
    // need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials (for testing)
    //* Activate for test mode */ wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name (
    // "AutoConnectAP"), if empty will auto generate SSID, if password is blank it
    // will be anonymous AP (wm.autoConnect()) then goes into a blocking loop
    // awaiting configuration and will return success result

    bool res;
    res = wm.autoConnect("Clock");  // password protected ap

    if (!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } else {
        Serial.printf("Connected to %s as %s\n", WiFi.SSID(), WiFi.localIP().toString());
    }
}

void wifiReset() {
    WiFiManager wm;
    wm.resetSettings();
}

void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_READY:
            Serial.println("[WiFi] Interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Serial.println("[WiFi] Completed scan");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("[WiFi] Client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("[WiFi] Clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("[WiFi] Connected to AP");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[WiFi] Disconnected");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("[WiFi] Authentication mode of AP changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("[WiFi] Obtained IP address: %s\n", WiFi.localIP().toString());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("[WiFi] Lost IP address");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("[WiFi] Protected Setup (WPS) enrolled");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("[WiFi] Protected Setup (WPS) failed");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("[WiFi] Protected Setup (WPS) timeout");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("[WiFi] Protected Setup (WPS): pin code");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("[WiFi] Access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("[WiFi] Access point stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("[WiFi] Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("[WiFi] Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("[WiFi] IP address assigned");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("[WiFi] Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("[WiFi] AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("[WiFi] STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("[WiFi] Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("[WiFi] Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("[WiFi] Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("[WiFi] Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("[WiFi] Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("[WiFi] Obtained IP address");
            break;
        default:
            break;
    }
}