#ifndef SETUP_H
#define SETUP_H

#include <WiFi.h>

#define SSID "***REMOVED***"
#define PASSWORD "***REMOVED***"
#define MQTT_SERVER "pi.home"

#define LED_BUILTIN 33
#define LED_FLASHLIGHT 4
#define BLINK_FREQUENCY 10
#define BLINK_DELAY_TIME 1000 / BLINK_FREQUENCY / 2

// OTA (ota.cpp)
void setupOTA();
void handleOTA();

// WiFi & WebApp (wifi.cpp)
void setupWiFi();
void setupWebServer();
void startWebServer();

// MQTT (mqtt.cpp)
void mqttSetup();
void mqttLoop();

// Camera (camera.cpp)
void setupCamera();

// LED & awakeTime (extra.cpp)
class Led
{
    uint8_t pin_;
    bool inverted_;
    bool led_on_;

public:
    Led(uint8_t pin);
    Led(uint8_t pin, bool invertedOutput);
    bool status();
    void off();
    void on();
    void toggle();
    void blink(uint8_t blinkCount, uint32_t delayTime = BLINK_DELAY_TIME);
};

static Led ledBuiltIn(LED_BUILTIN, true);
static Led flashlight(LED_FLASHLIGHT);

// Make Timer class
extern uint16_t stayAwakeTime;
void setStayAwakeTime(uint16_t time);
uint16_t getStayAwakeTime();

#endif