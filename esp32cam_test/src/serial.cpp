#include <Arduino.h>
#include "esp_camera.h"

void serialSetup(bool debugFlag)
{
    Serial.begin(115200);
    Serial.setDebugOutput(debugFlag);
    Serial.println();
}