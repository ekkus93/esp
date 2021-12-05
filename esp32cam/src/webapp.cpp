#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdlib.h> /* srand, rand */
#include <limits.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "SD_MMC.h"
#include "sdcard.h"
#include "webapp.h"
#include "led.h"
#include "camera.h"
#include "ntp.h"

#define PORT 80
static WebServer *webServer;
bool shouldReboot = false;
String urlBase = "http://192.168.88.106:3000";

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

void handleRoot()
{
  Serial.printf("###handleRoot - 1\n");
  whiteLedOn();
  webServer->send(200, F("text/html"), makeHtmlResp());
  whiteLedOff();
}

void onWhiteLightOn()
{
  whiteLedOn();
  webServer->send(200);
}

void onWhiteLightOff()
{
  whiteLedOff();
  webServer->send(200);
}

void onWhiteLightToggle()
{
  whiteLedToggle();
  webServer->send(200);
}

void onTakeImg()
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
    webServer->send(500);
  }
  else
  {
    DynamicJsonDocument doc(1024);
    doc["imgPath"] = path;
    String buf;
    serializeJson(doc, buf);
    webServer->send(200, F("application/json"), buf);
  }

  free(path);
}

String quoteEscapeStr(String str)
{
  return "\"" + str + "\"";
}

void onGetImgList()
{
  Serial.printf("###onGetImgList - 1\n");
  const char dirname[] = "/img";
  File root = SD_MMC.open(dirname);
  if (!root || !root.isDirectory())
  {
    Serial.printf("###onGetImgList - 2\n");
    webServer->send(500);
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

  DynamicJsonDocument doc(1024);
  doc["imgList"] = serialized("[" + fileListStr + "]");
  String buf;
  serializeJson(doc, buf);
  webServer->send(200, F("application/json"), buf);
}

String getContentType(String filename)
{
  if (webServer->hasArg("download"))
  { // check if the parameter "download" exists
    return "application/octet-stream";
  }
  else if (filename.endsWith(".htm"))
  { // check if the string filename ends with ".htm"
    return "text/html";
  }
  else if (filename.endsWith(".html"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".css"))
  {
    return "text/css";
  }
  else if (filename.endsWith(".js"))
  {
    return "application/javascript";
  }
  else if (filename.endsWith(".png"))
  {
    return "image/png";
  }
  else if (filename.endsWith(".gif"))
  {
    return "image/gif";
  }
  else if (filename.endsWith(".jpg"))
  {
    return "image/jpeg";
  }
  else if (filename.endsWith(".ico"))
  {
    return "image/x-icon";
  }
  else if (filename.endsWith(".xml"))
  {
    return "text/xml";
  }
  else if (filename.endsWith(".pdf"))
  {
    return "application/x-pdf";
  }
  else if (filename.endsWith(".zip"))
  {
    return "application/x-zip";
  }
  else if (filename.endsWith(".gz"))
  {
    return "application/x-gzip";
  }
  return "text/plain";
}

void fileSend(String path, String contentType)
{
  Serial.printf("###sendFile - 1\n");

  Serial.print("contentType ");
  Serial.println(contentType);

  Serial.printf("###sendFile - 2\n");
  dirList(path.c_str(), &dirPrint);

  Serial.printf("###sendFile - 3\n");
  File file = SD_MMC.open(path, "r"); // Open the File with file name = to path with intention to read it. For other modes see <a href="https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html" style="font-size: 13.5px;"> https://arduino-esp8266.readthedocs.io/en/latest/...</a>

  Serial.printf("###sendFile - 4\n");
  if (file.isDirectory())
  {
    Serial.printf("###sendFile - 5\n");
    file.close();

    contentType = "text/html";
    Serial.printf("###sendFile - 6\n");
    if (path.endsWith("/"))
    {
      Serial.printf("###sendFile - 7\n");
      file = SD_MMC.open(path + "index.html");
    }
    else
    {
      Serial.printf("###sendFile - 8\n");
      file = SD_MMC.open(path + "/index.html");
    }

    Serial.printf("###sendFile - 9\n");
    if (!file)
    {
      Serial.printf("###sendFile - 10\n");
      webServer->send(404);
      return;
    }
  }

  size_t sent = webServer->streamFile(file, contentType); // sends the file to the webServer references from <a href="https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h" style="font-size: 13.5px;"> https://arduino-esp8266.readthedocs.io/en/latest/...</a>
  Serial.printf("###sendFile: %d bytes\n", sent);
  file.close(); // Close the file
}

bool proxyRequest(String uri)
{
  char timeStr[11];
  sprintf(timeStr, "%lu", getTime());
  String tempFilePath = String("/tmp/") + String(timeStr);

  if (!dirSafeCreate("/tmp"))
  {
    return false;
  }

  File file = SD_MMC.open(tempFilePath, FILE_WRITE);

  HTTPClient http;
  Serial.printf("###proxyRequest - 3\n");
  http.begin(uri.c_str());

  Serial.printf("###proxyRequest - 4\n");

  // Send HTTP GET request
  int httpResponseCode = http.GET();
  if (httpResponseCode != 200)
  {
    webServer->send(httpResponseCode);
    http.end();
    return false;
  }

  WiFiClient *wifiClient = http.getStreamPtr();

  uint8_t buf[512];
  size_t byteCnt = 0;
  bool readFlag = true;

  Serial.printf("###proxyRequest - 11\n");

  while (readFlag)
  {
    // Serial.printf("###proxyRequest - 12\n");
    byteCnt = wifiClient->readBytes(buf, 512);
    if (byteCnt > 0)
    {
      // Serial.printf("###proxyRequest - 13 - byteCnt: %d\n", byteCnt);
      // Serial.printf("%s\n", (const char *)buf);
      file.write(buf, byteCnt);
    }
    else
    {
      // Serial.printf("###proxyRequest - 14\n");
      file.flush();
      file.close();
      readFlag = false;
    }
  }

  Serial.printf("###url: %s\n", uri.c_str());
  String contentType = getContentType(uri.c_str());
  Serial.printf("###contentType: %s\n", contentType.c_str());
  fileSend(tempFilePath, contentType);
  fileDelete(tempFilePath);

  // http.end();
  
  return true;
}

void onRequest()
{
  Serial.printf("###onRequest - 1\n");
  String uri = webServer->uri();
  if (uri.startsWith("/img"))
  {
    String contentType = getContentType(uri);
    fileSend(uri, contentType);
  }
  else if (uri.startsWith("/app"))
  {
    // sendFile(uri);
    proxyRequest(urlBase + uri);
  }
  else
  {
    // Handle Unknown Request
    webServer->send(404);
  }
}

WebServer *webAppSetup()
{+
  Serial.printf("###webAppSetu++p - 1\n");
  webServer = new WebServer(PORT);

  Serial.printf("###webAppSetup - 2\n");
  webServer->on("/", HTTP_GET, handleRoot);

  Serial.printf("###webAppSetup - 3\n");
  webServer->on("/api/whiteled/on", HTTP_GET, onWhiteLightOn);
  webServer->on("/api/whiteled/off", HTTP_GET, onWhiteLightOff);
  webServer->on("/api/whiteled/toggle", HTTP_GET, onWhiteLightToggle);

  Serial.printf("###webAppSetup - 4\n");
  webServer->on("/api/img/take", HTTP_GET, onTakeImg);
  webServer->on("/api/img/list", HTTP_GET, onGetImgList);

  Serial.printf("###webAppSetup - 5\n");
  webServer->onNotFound(onRequest);

  Serial.printf("###webAppSetup - 6\n");
  webServer->begin();
  Serial.println("HTTP webServer started");

  return webServer;
}
