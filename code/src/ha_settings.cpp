#include "ha_settings.h"

#include <ArduinoHA.h>
#include <WiFi.h>

#include "board_led.h"
#include "config_server.h"

boolean active = false;
WiFiClient wifiClient;
HADevice haDevice;
HAMqtt haServer(wifiClient, haDevice, 30);

HANumber flowTime("clock_duration");
HASelect clockFlow("clock_flow");
HASelect clockFace("clock_face");
HANumber faceType("clock_face_nr");
HANumber faceSpeed("clock_face_sp", HABaseDeviceType::NumberPrecision::PrecisionP1);
HALight clockColor("clock_color", HALight::Features::RGBFeature);
HALight tickColor("clock_color2", HALight::Features::RGBFeature);
HALight internalLed("clock_int_led", HALight::Features::RGBFeature);

HALight statusBarA("clock_status_A", HALight::Features::RGBFeature);
HALight statusBarB("clock_status_B", HALight::Features::RGBFeature);
HALight statusBarC("clock_status_C", HALight::Features::RGBFeature);
HALight statusBarD("clock_status_D", HALight::Features::RGBFeature);
HALight statusBarE("clock_status_E", HALight::Features::RGBFeature);
HALight statusBarF("clock_status_F", HALight::Features::RGBFeature);
HALight statusBarG("clock_status_G", HALight::Features::RGBFeature);
HALight statusBarH("clock_status_H", HALight::Features::RGBFeature);
HALight statusBarI("clock_status_I", HALight::Features::RGBFeature);
HALight statusBarJ("clock_status_J", HALight::Features::RGBFeature);
HALight statusBarK("clock_status_K", HALight::Features::RGBFeature);
HALight statusBarL("clock_status_L", HALight::Features::RGBFeature);
HALight statusBarM("clock_status_M", HALight::Features::RGBFeature);

HASensor ipAddress("clock_address");
HASensorNumber lightIntensity("clock_light", HABaseDeviceType::PrecisionP0);
HASwitch lightActive("clock_sensor");
HASelect clockStatus("clock_status");

// New settings, also update config_server en optionally switch_control !

BoardLed haLed = BoardLed();
WordClock* clockRef = NULL;

const char* baseName = "Status ?";
const char* ipAddressValue = "unknown";
char nameArray[13][10];

void loopMqtt() {
    if (active) {
        faceType.setState((int16_t)clockRef->getAnimation());
        if (WiFi.status() == WL_CONNECTED) haServer.loop();
    }
}

String getHomeAssistantStatus() {
    String avail = haDevice.isAvailable() ? "Yes, and online" : "Yes, but offline";
    return haServer.isConnected() ? avail : "<span style=\"color:Crimson; font-weight:bold\">No</span>";
}

int getSenderIndex(HALight* sender) {
    const char* name = sender->uniqueId();
    if (name == NULL) return -1;
    int index = strlen(name) - 1;
    if (index < 0) return -1;
    // Zero-based index
    return name[index] - 'A';
}

void onSwitchCommand(bool state, HALight* sender) {
    if (sender == &internalLed) {
        if (state) {
            haLed.turnOn();
            sender->setRGBColor(HALight::RGBColor(haLed.getRed(), haLed.getGreen(), haLed.getBlue()));
        } else {
            haLed.turnOff();
        }
    } else if (sender == &clockColor) {
        clockRef->setVisible(state);
    } else if (sender == &tickColor) {
        clockRef->setTickVisible(state);
    } else {
        int idx = getSenderIndex(sender);
        clockRef->activateStatusLed(idx, state);
    }
    sender->setState(state);
}

void onColorCommand(HALight::RGBColor color, HALight* sender) {
    if (sender == &internalLed) {
        haLed.activate(color.red, color.green, color.blue);
    } else if (sender == &clockColor) {
        clockRef->setColor(color.red, color.green, color.blue);
    } else if (sender == &tickColor) {
        clockRef->setTickColor(color.red, color.green, color.blue);
    } else {
        int idx = getSenderIndex(sender);
        // Serial.printf("Setting status %d to %d,%d,%d\n", idx, color.red, color.green, color.blue);
        clockRef->setStatusColor(idx, color.red, color.green, color.blue);
    }
    sender->setRGBColor(color);
}

void onSelectCommand(int8_t index, HASelect* sender) {
    if (sender == &clockFlow)
        clockRef->setFlowPattern(index);
    else if (sender == &clockFace)
        clockRef->setFacePattern(index);
    else if (sender == &clockStatus)
        clockRef->setStatusPattern(index);

    sender->setState(index);
}

void onSwitchCommand(bool state, HASwitch* sender) {
    clockRef->setLightSensorState(state);
    sender->setState(state);
}

void onSetTransitionTimeCommand(HANumeric number, HANumber* sender) {
    if (number.isSet()) {
        clockRef->setFlowTime(number.toUInt16());
        sender->setState(number);
    }
}

void onSetValue(HANumeric number, HANumber* sender) {
    if (number.isSet()) {
        if (sender == &faceType)
            clockRef->setAnimation(number.toInt16());
        else if (sender == &faceSpeed)
            clockRef->setAnimationSpeed(number.toFloat());
        sender->setState(number);
    }
}

void updateAnalog(int percentage) {
    if (active) lightIntensity.setValue((int16_t)percentage);
}

void updateIP(const char* address) {
    if (active && strcmp(address, ipAddressValue) > 0) {
        ipAddressValue = address;
        ipAddress.setValue(address);
    }
}

HALight::RGBColor rgb(CRGB input) { return HALight::RGBColor(input.r, input.g, input.b); }

void printMessage(const char* topic, const uint8_t* payload, uint16_t length) {
    char message[length + 1];
    for (size_t i = 0; i < length; i++) message[i] = payload[i];
    message[length] = 0;
    Serial.printf("[%s]: %s\n", topic, message);
}

void setupHomeAssist(WordClock* clock, ConfigServer* cfg) {
    uint8_t mac[6] = {0xB6, 0x33, 0x80, 0xFB, 0xCD, 0x4A};

    if (!cfg->hasMqttServer()) return;

    // Unique code based on device name
    for (size_t i = 0; i < 6; i++) {
        char x = cfg->getDeviceName()[i];
        if (x == 0) break;
        mac[i] = x;
    }

    haDevice.setUniqueId(mac, sizeof(mac));
    haDevice.setName(cfg->getDeviceName());
    haDevice.setSoftwareVersion("1.1.0");
    haDevice.setManufacturer("Lemval Systems");
    haDevice.enableExtendedUniqueIds();
    haDevice.enableSharedAvailability();
    haDevice.enableLastWill();

    clockRef = clock;

    internalLed.setName("Internal LED");
    internalLed.setIcon("mdi:lightbulb");
    internalLed.onStateCommand(onSwitchCommand);
    internalLed.onRGBColorCommand(onColorCommand);

    clockColor.setName("Color");
    clockColor.setIcon("mdi:lightbulb");
    clockColor.onStateCommand(onSwitchCommand);
    clockColor.onRGBColorCommand(onColorCommand);

    tickColor.setName("Seconds");
    tickColor.setIcon("mdi:lightbulb");
    tickColor.onStateCommand(onSwitchCommand);
    tickColor.onRGBColorCommand(onColorCommand);

    clockFlow.setName("Flow");
    clockFlow.setIcon("mdi:car-shift-pattern");
    clockFlow.setOptions(FlowOption::getAsString());
    clockFlow.onCommand(onSelectCommand);

    clockFace.setName("Face");
    clockFace.setIcon("mdi:car-shift-pattern");
    clockFace.setOptions(FaceOption::getAsString());
    clockFace.onCommand(onSelectCommand);

    clockStatus.setName("Status");
    clockStatus.setIcon("mdi:car-shift-pattern");
    clockStatus.setOptions(StatusOption::getAsString());
    clockStatus.onCommand(onSelectCommand);

    flowTime.setName("Slide time");
    flowTime.setIcon("mdi:timer-check-outline");
    flowTime.setMin(0);
    flowTime.setMax(5000);
    flowTime.setStep(100);
    flowTime.setMode(HANumber::ModeSlider);
    flowTime.setUnitOfMeasurement("ms");
    flowTime.onCommand(onSetTransitionTimeCommand);

    faceType.setName("Animation");
    faceType.setMin(-1);
    faceType.setMax(50);
    faceType.setStep(1.0);
    faceType.setMode(HANumber::ModeSlider);
    faceType.onCommand(onSetValue);
    faceType.setCurrentState(-1.0f);

    faceSpeed.setName("Animation Speed");
    faceSpeed.setMin(0.1);
    faceSpeed.setMax(2.5);
    faceSpeed.setStep(0.1);
    faceSpeed.setMode(HANumber::ModeSlider);
    faceSpeed.onCommand(onSetValue);
    faceSpeed.setCurrentState(0.9f);

    lightIntensity.setName("Light Level");
    lightIntensity.setIcon("mdi:brightness-4");

    lightActive.setName("Light sensor");
    lightActive.onCommand(onSwitchCommand);
    lightActive.setIcon("mdi:brightness-4");

    ipAddress.setName("IP Address");
    ipAddress.setIcon("mdi:lan");
    int i = 0;

    for (HALight* light : {
             &statusBarA,
             &statusBarB,
             &statusBarC,
             &statusBarD,
             &statusBarE,
             &statusBarF,
             &statusBarG,
             &statusBarH,
             &statusBarI,
             &statusBarJ,
             &statusBarK,
             &statusBarL,
             &statusBarM,
         }) {
        strcpy(nameArray[i], baseName);
        nameArray[i][7] = 'A' + i;
        light->setName(nameArray[i]);
        light->setIcon("mdi:star-circle-outline");
        light->onStateCommand(onSwitchCommand);
        light->onRGBColorCommand(onColorCommand);
        light->setCurrentState(0);
        light->setCurrentRGBColor(HALight::RGBColor(0x0, 0x0, 0x0));
        i = i + 1;
        //*/ Temporary */ light->setRetain(false);

        active = true;
    }

    Serial.printf("Home Assistant on MQTT %s:%d\n", cfg->getMqttServer(), cfg->getMqttPort());

    haServer.setDataPrefix("lemval");
    haServer.begin(cfg->getMqttServer(), cfg->getMqttPort(), cfg->getMqttUser(), cfg->getMqttPassword());
    loopMqtt();
    // haServer.onMessage(printMessage);

    // Send status to HA
    for (HALight* light : {
             &statusBarA,
             &statusBarB,
             &statusBarC,
             &statusBarD,
             &statusBarE,
             &statusBarF,
             &statusBarG,
             &statusBarH,
             &statusBarI,
             &statusBarJ,
             &statusBarK,
             &statusBarL,
             &statusBarM,
         }) {
        light->setState(0);
    }

    tickColor.setRGBColor(rgb(clock->getTickColor()), true);
    tickColor.setState(clock->getTickState(), true);

    clockColor.setRGBColor(rgb(clock->getCustomColor()), true);
    clockColor.setState(1);

    clockFlow.setState(clock->getFlowOption(), true);
    clockFace.setState(clock->getFaceOption(), true);
    clockStatus.setState(clock->getStatusOption(), true);
    flowTime.setState((int16_t)clock->getFlowTime(), true);
    ipAddress.setValue(wifiClient.localIP().toString().c_str());
    lightActive.setState(clock->getLightSensorState(), true);

    loopMqtt();
}

void updateSettings(FlowOption flow, FaceOption face, bool tick, bool sensor, int16_t time) {
    if (active) {
        clockFlow.setState(flow.getValue());
        clockFace.setState(face.getValue());
        tickColor.setState(tick);
        lightActive.setState(sensor);
        flowTime.setState(time, true);
    }
}
