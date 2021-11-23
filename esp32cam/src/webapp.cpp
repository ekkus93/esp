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
  whiteLedOn();
  server->send(200, "text/html", makeHtmlResp());
  whiteLedOff();
}

void webAppSetup()
{
  server = new WebServer(PORT);

  server->on("/", handleRoot);
  server->begin();
  Serial.println("HTTP server started");
}

void handleClient()
{
  server->handleClient();
}