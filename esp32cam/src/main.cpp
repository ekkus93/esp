#include <Arduino.h>
#include "esp_camera.h"
#include "SD_MMC.h"
#include "sdcard.h"
#include "camera.h"
#include "serial.h"
#include "led.h"
#include "wifi_manager.h"
#include "webapp.h"
#include "ntp.h"

bool haltFlag = false;
bool debugFlag = true;

void setup()
{
  serialSetup(debugFlag);

  ledSetup();

  if (ESP_OK != sdcardInit())
  {
    haltFlag = true;
    return;
  }

  cameraInit();

  // wifiApSetup();
  IPAddress ipAddress = wifiStaSetup();
  Serial.printf("\nIP address: %s\n", ipAddress.toString().c_str());

  webAppSetup();

  ntpSetup();
  Serial.printf("time: %lu\n", getTime());
  delay(5000);
  ntpSetup();
  Serial.printf("time: %lu\n", getTime());
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