#include "Arduino.h"
#include <WiFi.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#define LED_BUILTIN 33

const char *ssid = "***REMOVED***";
const char *password = "***REMOVED***";

void startWebServer();
// void setup();
// void loop();

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(10000);
}