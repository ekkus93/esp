#include "time.h"
#include <WiFi.h>

void ntpSetup()
{
    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time-a-g.nist.gov";
    const char *ntpServer3 = "utcnist.colorado.edu";

    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 3600;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
}

unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        //Serial.println("Failed to obtain time");
        return (0);
    }
    time(&now);
    return now;
}