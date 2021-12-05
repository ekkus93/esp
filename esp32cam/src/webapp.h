#ifndef WEBAPP_H
#define WEBAPP_H

#include <WebServer.h>

WebServer *webAppSetup();
void handleRoot();
void onWhiteLightOn();
void onWhiteLightOff();
void onWhiteLightToggle();
void onTakeImg();
String quoteEscapeStr(String str);
void onGetImgList();
String getContentType(String filename);
void fileSend(String path, String contentType);
bool proxyRequest(String uri);
void onRequest();
String makeHtmlResp();

#endif