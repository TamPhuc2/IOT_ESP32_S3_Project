#include "task_wifi.h"

void init_wifi(SystemHandles* handles) {
    // Load config từ flash
    Preferences prefs;
    prefs.begin("config", true);
    handles->sysData.wifi_ssid = prefs.getString("ssid", "");
    handles->sysData.wifi_pass = prefs.getString("pass", "");
    prefs.end();

    // handles->sysData.wifi_ssid = "hts";
    // handles->sysData.wifi_pass = "11111111"; 
    // handles->sysData.coreiot_server = "app.coreiot.io";
    // handles->sysData.coreiot_port = 1883U; 
    // handles->sysData.coreiot_token = "7d8thcolc3gb7jirlyp1";

    if (handles->sysData.wifi_ssid.isEmpty()) {
        // Chưa có cấu hình -> bật AP
        WiFi.mode(WIFI_AP);
        WiFi.softAP(handles->sysData.ap_ssid.c_str(), handles->sysData.ap_pass.c_str());
        Serial.println("[WiFi] AP mode started, IP: " + WiFi.softAPIP().toString());
    } else {
        // Đã có cấu hình -> chạy STA
        WiFi.mode(WIFI_STA);
        WiFi.begin(handles->sysData.wifi_ssid.c_str(), handles->sysData.wifi_pass.c_str());
        Serial.println("[WiFi] STA mode connecting to " + handles->sysData.wifi_ssid);
    }
}

void wifi_task(void *pvParameters) {
    SystemHandles* handles = (SystemHandles*)pvParameters;
    for (;;) {
        if (WiFi.getMode() == WIFI_STA) {
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("[WiFi] Reconnecting STA...");
                WiFi.disconnect();
                WiFi.begin(handles->sysData.wifi_ssid.c_str(), handles->sysData.wifi_pass.c_str());
                int retry = 0;
                while (WiFi.status() != WL_CONNECTED && retry < 20) {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    Serial.print(".");
                    retry++;
                }
                if (WiFi.status() == WL_CONNECTED) {
                    Serial.println("\n[WiFi] Connected, IP: " + WiFi.localIP().toString());
                } else {
                    Serial.println("\n[WiFi] Failed to connect.");
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}