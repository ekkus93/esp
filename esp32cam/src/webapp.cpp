#include <Arduino.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <stdlib.h> /* srand, rand */
#include <limits.h>
#include "SD_MMC.h"
#include "webapp.h"
#include "led.h"
#include "camera.h"
#include "ntp.h"

#define PORT 80

static AsyncWebServer *server = NULL;
static AsyncWebSocket *ws = NULL;
static AsyncEventSource *events = NULL;

bool shouldReboot = false;

String makeHtmlResp()
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>test</title>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Hello, world</h1>\n";
  ptr += "</body>\n";

  return ptr;
}

void handleRoot(AsyncWebServerRequest *request)
{
  whiteLedOn();
  request->send(200, "text/html", makeHtmlResp());
  whiteLedOff();
}

void onRequest(AsyncWebServerRequest *request)
{
  //Handle Unknown Request
  request->send(404);
}

void onWhiteLightOn(AsyncWebServerRequest *request)
{
  whiteLedOn();
  request->send(200);
}

void onWhiteLightOff(AsyncWebServerRequest *request)
{
  whiteLedOff();
  request->send(200);
}

void onWhiteLightToggle(AsyncWebServerRequest *request)
{
  whiteLedToggle();
  request->send(200);
}

void onTakeImg(AsyncWebServerRequest *request)
{
  unsigned long currTime = getTime();
  if (currTime == 0)
  {
    currTime = rand() % ULONG_MAX;
  }

  char *path = (char *)malloc(sizeof(char) * 30);
  sprintf(path, "/img/%lu.jpg", currTime);

  if (takePic(path) != ESP_OK)
  {
    request->send(500);
  }
  else
  {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument doc(1024);
    doc["imgPath"] = path;
    serializeJson(doc, *response);
    request->send(response);
  }

  free(path);
}

String quoteEscapeStr(String str)
{
  return "\"" + str + "\"";
}

void onGetImgList(AsyncWebServerRequest *request)
{
  Serial.printf("###onGetImgList - 1\n");
  const char dirname[] = "/img";
  File root = SD_MMC.open(dirname);
  if (!root || !root.isDirectory())
  {
    Serial.printf("###onGetImgList - 2\n");
    request->send(500);
    return;
  }
  Serial.printf("###onGetImgList - 3\n");

  String fileListStr = String("");
  File file = root.openNextFile();
  Serial.printf("###onGetImgList - 4\n");
  while (file)
  {
    Serial.printf("###onGetImgList - 5\n");
    if (!file.isDirectory())
    {
      Serial.printf("###onGetImgList - 6\n");
      String fileName = String(file.name());
      if (fileName.endsWith(".jpg"))
      {
        Serial.printf("###onGetImgList - 7\n");
        if (!fileListStr.equals(""))
        {
          Serial.printf("###onGetImgList - 8\n");
          fileListStr += ",";
        }
        Serial.printf("###onGetImgList - 9\n");
        fileListStr += quoteEscapeStr(fileName);
      }
      Serial.printf("###onGetImgList - 10\n");
    }
    Serial.printf("###onGetImgList - 11\n");
    file.close();
    Serial.printf("###onGetImgList - 11a\n");
    file = root.openNextFile();
    Serial.printf("###onGetImgList - 11b\n");
  }

  Serial.printf("###onGetImgList - 12\n");
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  Serial.printf("###onGetImgList - 13\n");
  DynamicJsonDocument doc(1024);
  Serial.printf("###onGetImgList - 14\n");
  doc["imgList"] = serialized("[" + fileListStr + "]");
  Serial.printf("###onGetImgList - 15\n");
  serializeJson(doc, *response);
  Serial.printf("###onGetImgList - 16\n");
  request->send(response);
  Serial.printf("###onGetImgList - 17\n");
}

void webAppSetup()
{
  server = new AsyncWebServer(PORT);
  ws = new AsyncWebSocket("/ws");
  events = new AsyncEventSource("/events");

  server->on("/", HTTP_GET, handleRoot);

  server->on("/api/whiteled/on", HTTP_GET, onWhiteLightOn);
  server->on("/api/whiteled/off", HTTP_GET, onWhiteLightOff);
  server->on("/api/whiteled/toggle", HTTP_GET, onWhiteLightToggle);

  server->on("/api/img/take", HTTP_GET, onTakeImg);
  server->on("/api/img/list", HTTP_GET, onGetImgList);

  server->serveStatic("/img", SD_MMC, "/img");
  server->serveStatic("/app", SD_MMC, "/app");
  server->serveStatic("/app/", SD_MMC, "/app/index.html");

  server->onNotFound(onRequest);

  server->begin();
  Serial.println("HTTP server started");
}
