#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "time.h"

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define INITIAL_AWAKE_TIME 15     /* Time ESP32 will stay awake for (in seconds) */
#define TIME_TO_SLEEP 60          /* Time ESP32 will go to sleep (in seconds) */

#define LED_BUILTIN 33
#define BLINK_FREQUENCY 10
#define BLINK_DELAY_TIME 1000 / BLINK_FREQUENCY / 2

RTC_DATA_ATTR int bootCount = 0;

// Network
const char *ssid = "***REMOVED***";
const char *password = "***REMOVED***";
const char *mqtt_server = "pi.home";

// Current time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // GMT+1h for Sweden
const int daylightOffset_sec = 3600;

uint16_t stayAwakeTime;

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void startWebServer();
void turn_led_on();
void turn_led_off();

void setStayAwakeTime(uint16_t time)
{
  stayAwakeTime = time;
}

void blinkLED(uint8_t pin, uint8_t blinkCount, uint32_t delayTime = BLINK_DELAY_TIME)
{
  Serial.printf("Blinking LED %d times in %.2f seconds\n", blinkCount, blinkCount * delayTime * 2 / 1000.0);
  for (size_t i = 0; i < blinkCount; i++)
  {
    // digitalWrite(LED_BUILTIN, LOW); // ON | Inverted logic
    turn_led_on();
    delay(delayTime);
    // digitalWrite(LED_BUILTIN, HIGH); // OFF | Inverted logic
    turn_led_off();
    delay(delayTime);
  }
}

void setupWiFi()
{
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
}

void mqttCallback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic ESP-Cam/in, you check if the message is either "fast" or "slow".
  // Changes the output state according to the message
  if (String(topic) == "ESP-Cam/in")
  {
    if (messageTemp == "fast")
      blinkLED(LED_BUILTIN, 10);

    else if (messageTemp == "slow")
      blinkLED(LED_BUILTIN, 10, 250);

    else if (messageTemp.startsWith("stay awake"))
    {
      if (messageTemp.startsWith("stay awake:"))
      {
        String numberString = messageTemp.substring(11, messageTemp.length());
        int newTime = numberString.toInt();
        Serial.println(newTime);
        if (newTime > 0)
          setStayAwakeTime(newTime);
      }
      else
        setStayAwakeTime(30);
    }

    else if (messageTemp == "sleep")
      esp_deep_sleep_start();

    else
    {
      char msgReply[60];
      if (messageTemp.length() > 35)
      {
        messageTemp = messageTemp.substring(0, 30) + "[...]";
      }
      sprintf(msgReply, "Unknown command '%s'", messageTemp.c_str());
      client.publish("ESP-Cam/out", msgReply); // awake: 01/15/22 18:32:28
    }
  }
}

void mqttReconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a client ID using the MAC address
    String clientId = "ESP32Client-";
    clientId += String(WiFi.macAddress());
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      struct tm timeinfo;
      char currentTime[30];

      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      getLocalTime(&timeinfo);
      strftime(currentTime, 20, "%x %X", &timeinfo);
      String awakeMsg = "awake: ";
      awakeMsg += String(currentTime);

      client.publish("ESP-Cam/out", awakeMsg.c_str()); // awake: 01/15/22 18:32:28
      // ... and resubscribe
      client.subscribe("ESP-Cam/in");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}

void mqttSetup()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
}

void mqttLoop()
{
  if (!client.connected())
  {
    mqttReconnect();
  }
  client.loop();
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for " + String(TIME_TO_SLEEP) + " Seconds");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Inverted logic; keep it off at start

  setupWiFi();

  Serial.printf("Will stay awake for %d seconds\n", INITIAL_AWAKE_TIME);
  stayAwakeTime = INITIAL_AWAKE_TIME; // Stay awake INITIAL_AWAKE_TIME seconds

  mqttSetup();

  while (stayAwakeTime)
  {
    mqttLoop();

    delay(1000);
    stayAwakeTime--;
    if (stayAwakeTime % 10 == 0)
    {
      Serial.printf("Awake time remaining: %d s\n", stayAwakeTime);
    }
  }

  blinkLED(LED_BUILTIN, 5);
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop()
{
  //This is not going to be called
}