#include "mainserver.h"
#include <WiFi.h>
#include <WebServer.h>

bool led1_state = false;
bool led2_state = false;
bool isAPMode = true;

WebServer server(80);

unsigned long connect_start_ms = 0;
bool connecting = false;

// ========== Handlers ==========

void handleSensors()
{
  float t = glob_temperature;
  float h = glob_humidity;
  String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
  server.send(200, "application/json", json);
}

void handlePower() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    if (state == "on") {
      digitalWrite(POWER_PIN, HIGH);  
      server.send(200, "text/plain", "Power ON");
    } else if (state == "off") {
      digitalWrite(POWER_PIN, LOW);
      server.send(200, "text/plain", "Power OFF");
    } else {
      server.send(400, "text/plain", "Invalid state");
    }
  } else {
    server.send(400, "text/plain", "Missing state");
  }
}

void handleLed() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    if (state == "on") {
      digitalWrite(LED_PIN, HIGH);
      server.send(200, "text/plain", "LED ON");
    } else if (state == "off") {
      digitalWrite(LED_PIN, LOW);
      server.send(200, "text/plain", "LED OFF");
    } else {
      server.send(400, "text/plain", "Invalid state");
    }
  } else {
    server.send(400, "text/plain", "Missing state");
  }
}
void handleConnect()
{
  wifi_ssid = server.arg("ssid");
  wifi_password = server.arg("pass");
  server.send(200, "text/plain", "Connecting....");
  isAPMode = false;
  connecting = true;
  connect_start_ms = millis();
  connectToWiFi();
}

// ========== WiFi ==========
void setupServer()
{
  SPIFFS.begin();

  // Route trả về index.html
  server.on("/", [](){
    File file = SPIFFS.open("/index.html", "r");
    if(file){
      server.streamFile(file, "text/html");
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  });

  // Route trả về style.css
  server.on("/style.css", [](){
    File file = SPIFFS.open("/style.css", "r");
    if(file){
      server.streamFile(file, "text/css");
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  });

  // Route trả về script.js
  server.on("/script.js", [](){
    File file = SPIFFS.open("/script.js", "r");
    if(file){
      server.streamFile(file, "application/javascript");
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  });
  server.on("/sensors", HTTP_GET, handleSensors);
  server.on("/power", handlePower);
  server.on("/led", handleLed);

  server.begin();
}

void startAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  // Serial.print("AP IP address: ");
  // Serial.println(WiFi.softAPIP());
  isAPMode = true;
  connecting = false;
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  if (wifi_password.isEmpty())
  {
    WiFi.begin(wifi_ssid.c_str());
  }
  else
  {
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  }
  Serial.print("Connecting to: ");
  Serial.print(wifi_ssid.c_str());

  Serial.print(" Password: ");
  Serial.print(wifi_password.c_str());
}

// ========== Main task ==========
void main_server_task(void *pvParameters)
{
  pinMode(BOOT_PIN, INPUT_PULLUP);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  while (1)
  {
    server.handleClient();
    startAP();
    setupServer();
    vTaskDelay(200); // avoid watchdog reset
  }
}