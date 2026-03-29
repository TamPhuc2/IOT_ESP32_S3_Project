#ifndef ___MAIN_SERVER__
#define ___MAIN_SERVER__
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "global.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#define POWER_PIN 47
#define LED_PIN 38
#define BOOT_PIN 0
//extern WebServer server;

//extern bool isAPMode;


void startAP();
void setupServer();
void connectToWiFi();

void main_server_task(void *pvParameters);

#endif