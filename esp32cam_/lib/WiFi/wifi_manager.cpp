#include "wifi_manager.h"
#include "WiFi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ESP32Ping.h>

WiFiManager::WiFiManager(const char *ssid, const char *password)
{
    this->ssid = (char *)calloc(strlen(ssid)+1, sizeof(char));
    strcpy(this->ssid, ssid);

    this->password = (char *)calloc(strlen(password)+1, sizeof(char));
    strcpy(this->password, password);
}

bool WiFiManager::connect() 
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    return true;
}

bool WiFiManager::disconnect()
{
    return WiFi.disconnect(true, true);
}

bool WiFiManager::ping(const char *host)
{
    return Ping.ping(host, 5);
}

WiFiManager::~WiFiManager()
{
    if (ssid != NULL) {
        free(ssid);
        ssid = NULL;
    }

    if (password != NULL) {
        free(password);
        ssid = NULL;
    }
}