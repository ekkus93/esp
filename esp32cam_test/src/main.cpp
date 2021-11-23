#include <Arduino.h>
#include "esp_camera.h"
#include "SD_MMC.h"
#include "sdcard.h"
#include "camera.h"
#include "serial.h"
#include "led.h"
#include "wifi_manager.h"
#include "webapp.h"

bool haltFlag = false;
bool debugFlag = true;

void setup()
{
  serialSetup(debugFlag);

  Serial.printf("###setup - 1\n");
  ledSetup();

  Serial.printf("###setup - 2\n");
  if (ESP_OK != sdcardInit())
  {
    Serial.printf("###setup - 3\n");
    haltFlag = true;
    return;
  }

  Serial.printf("###setup - 4\n");
  cameraInit();

  Serial.printf("###setup - 5\n");
  apSetup();

  Serial.printf("###setup - 6\n");
  webAppSetup();
  Serial.printf("###setup - 7\n");
}

void loop()
{
  /*
  char path[20];
  delay(500);

  for (int i = 0; i < 10; i++)
  {
    sprintf(path, "/test_cam_%d.jpg", i);
    if (ESP_OK != takePic(path))
    {
      ESP.restart();
    }

    delay(1000);
  }
  */

  handleClient();

  /*
  redLedOn();
  delay(500);
  redLedOff();
  delay(1000);
  */
}