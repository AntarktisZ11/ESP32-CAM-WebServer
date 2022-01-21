#include <common_setup.h>
#include <PubSubClient.h>
#include <time.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // GMT+1h for Sweden
const int daylightOffset_sec = 3600;

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
            ledBuiltIn.blink(10);

        else if (messageTemp == "slow")
            ledBuiltIn.blink(10, 250);

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
    client.setServer(MQTT_SERVER, 1883);
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
