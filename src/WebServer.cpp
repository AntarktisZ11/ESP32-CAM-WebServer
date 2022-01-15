#include "Arduino.h"
#include <WiFi.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 10          /* Time ESP32 will go to sleep (in seconds) */
#define AWAKE_TIME 40             /* Time ESP32 will stay awake for (in seconds) */

#define LED_BUILTIN 33
#define BLINK_FREQUENCY 3
#define BLINK_DELAY_TIME 1000 / BLINK_FREQUENCY / 2

RTC_DATA_ATTR int bootCount = 0;

const char *ssid = "NETGEAR46";
const char *password = "shinybox345";

void startWebServer();

void blinkLED(uint8_t pin, uint8_t blinkCount, uint32_t delayTime = BLINK_DELAY_TIME)
{
  Serial.printf("Blinking LED %d times in %d seconds\n", blinkCount, blinkCount / BLINK_FREQUENCY);
  for (size_t i = 0; i < blinkCount; i++)
  {
    digitalWrite(LED_BUILTIN, LOW); // ON | Inverted logic
    delay(delayTime);
    digitalWrite(LED_BUILTIN, HIGH); // OFF | Inverted logic
    delay(delayTime);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Inverted logic; keep it off at start

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startWebServer();

  Serial.print("Server Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  Serial.printf("Will stay awake for %d seconds\n", AWAKE_TIME);
  delay(1000 * AWAKE_TIME);

  blinkLED(LED_BUILTIN, 10, 250);

  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop()
{
  //This is not going to be called
}