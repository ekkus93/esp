#include <Arduino.h>
#include <unity.h>
#include "wifi_manager.h"

WiFiManager *wifiManager = NULL; 

void setup(void)
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    const char *ssid = "CircuitLaunch";
    const char *password = "makinghardwarelesshard";

    /*    
    const char *ssid = "kensington2";
    const char *password = "blu31tup*";
    */

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