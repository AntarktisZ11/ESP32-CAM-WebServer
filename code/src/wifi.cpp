#include <common_setup.h>

void setupWiFi()
{
    WiFi.begin(SSID, PASSWORD);

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Serial.println("");
    Serial.println("WiFi connected");
}

void setupWebServer()
{
    startWebServer();

    Serial.print("Server Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");
}
