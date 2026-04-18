#include "task_wifi.h"

constexpr char WIFI_SSID[] = "AMERICANO COFFEE";
constexpr char WIFI_PASSWORD[] = "";

void init_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void wifi_task(void *pvParameters) {
    SystemHandles* handles = (SystemHandles*)pvParameters;
    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Reconnecting...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            vTaskDelay(pdMS_TO_TICKS(500));
            Serial.print(".");
        }
        Serial.println("[WiFi] Connected!");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}