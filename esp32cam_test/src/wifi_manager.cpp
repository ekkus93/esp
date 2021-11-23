#include <WiFi.h>
#include "wifi_manager.h"

const char* apSsid = "e32c";
const char* apPassword = "3sp32cam*";

IPAddress local_ip(192,168,51,1);
IPAddress gateway(192,168,51,1);
IPAddress subnet(255,255,255,0);

void apSetup()
{
  WiFi.softAP(apSsid, apPassword);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);    
}