#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WiFiManager
{
    public:
        char *ssid;
        char *password;
        WiFiManager(const char *ssid, const char *password);
        bool connect();
        bool disconnect();
        bool ping(const char *host);
        ~WiFiManager();
};

#endif