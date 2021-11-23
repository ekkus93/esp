#include "webapp.h"
#include "led.h"

#define PORT 80

static WebServer *server = NULL;

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
  server->send(200, "text/html", makeHtmlResp());
  Serial.printf("###handleRoot - 2\n");
  whiteLedOff();
}

void webAppSetup()
{
  Serial.printf("###WebApp - 1\n");
  server = new WebServer(PORT);
  Serial.printf("###WebApp - 2\n");

  server->on("/", handleRoot);
  server->begin();
  Serial.println("HTTP server started");
  Serial.printf("###WebApp - 3\n");
}

void handleClient()
{
  //Serial.printf("###handleClient - 1\n");
  server->handleClient();
  //Serial.printf("###handleClient - 2\n");
}