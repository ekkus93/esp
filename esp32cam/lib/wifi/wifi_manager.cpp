#include "wifi_manager.h"

const char *apSsid = "e32c";
const char *apPassword = "3sp32cam*";
/*
const char *staSsid = "CircuitLaunch";
const char *staPassword = "makinghardwarelesshard";
*/
const char *staSsid = "kensington2";
const char *staPassword = "blu31tup*";

IPAddress local_ip(192, 168, 51, 1);
IPAddress gateway(192, 168, 51, 1);
IPAddress subnet(255, 255, 255, 0);

void wifiApSetup()
{
    WiFi.softAP(apSsid, apPassword);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);
}

IPAddress wifiStaSetup()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(staSsid, staPassword);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }

    return WiFi.localIP();
}