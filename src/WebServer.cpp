#include <Arduino.h>
#include <common_setup.h>

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define INITIAL_AWAKE_TIME 15     /* Time ESP32 will stay awake for (in seconds) */
#define TIME_TO_SLEEP 15          /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR uint16_t sleepTime = TIME_TO_SLEEP;

int setSleepTime(uint16_t time)
{
  // The return values should be switched, a normal exit is '0'
  if (time > 60 * 15)
    return 1;

  sleepTime = time;
  esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
  Serial.printf("Sleep duration now changed to %u\n", sleepTime);
  return 0;
}

uint16_t getSleepTime()
{
  return sleepTime;
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  // sleepTime += bootCount * 5;
  esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for " + String(sleepTime) + " Seconds");

  // Measurement has to be done before WiFi is used due to ADC1 being used
  measureBatteryLevel();

  setupCamera();

  setupWiFi();
  setupOTA();

  setupWebServer();

  Serial.printf("Will stay awake for %d seconds\n", INITIAL_AWAKE_TIME);
  stayAwakeTime = INITIAL_AWAKE_TIME; // Stay awake INITIAL_AWAKE_TIME seconds

  mqttSetup();

  while (stayAwakeTime)
  {
    mqttLoop();
    handleOTA();

    // Replace simple delay with timeStart + awakeTime < timeNow
    delay(1000);
    stayAwakeTime--;
    if (stayAwakeTime % 10 == 0)
    {
      Serial.printf("Awake time remaining: %d s\n", stayAwakeTime);
    }
  }

  ledBuiltIn.blink(5);
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop()
{
  // This is not going to be called
}