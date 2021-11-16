#include <Arduino.h>
#include <unity.h>
#include "wifi_manager.h"

WiFiManager *wifiManager = NULL; 

void setup(void)
{
    const char *ssid = "CircuitLaunch";
    const char *password = "makinghardwarelesshard";
    wifiManager = new WiFiManager(ssid, password);
    wifiManager->connect();
}

void tearDown(void)
{
    wifiManager->disconnect();
}

void test_ping()
{
    TEST_ASSERT_EQUAL(true, wifiManager->ping("google.com"));
}

void loop()
{
    UNITY_BEGIN();
    RUN_TEST(test_ping);
    UNITY_END(); // stop unit testing
}