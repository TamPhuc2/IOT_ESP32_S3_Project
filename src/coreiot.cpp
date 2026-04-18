#include "coreiot.h"

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr char LED_STATE_ATTR[] = "ledState";
constexpr char BLINKING_INTERVAL_ATTR[] = "blinkingInterval";

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

volatile bool attributesChanged = false;
volatile bool ledState = false;
volatile uint16_t blinkingInterval = 1000U;
uint32_t previousDataSend = 0;
constexpr int16_t telemetrySendInterval = 10000U;

RPC_Response setLedSwitchState(const RPC_Data &data){
    bool newState = data;
    Serial.println("Switch state change: ");
    Serial.print(newState);
    digitalWrite(48, newState);
    attributesChanged = true;
    return RPC_Response("setLedSwitchValue", newState);
}

const std::array<RPC_Callback, 1U> callbacks = {
    RPC_Callback{ "setLedSwitchValue", setLedSwitchState }
};

void processSharedAttributes(const Shared_Attribute_Data &data) {
    for (auto it = data.begin(); it != data.end(); ++it){
        if (strcmp(it->key().c_str(), BLINKING_INTERVAL_ATTR) == 0) {
            blinkingInterval = it->value().as<uint16_t>();
        } else if (strcmp(it->key().c_str(), LED_STATE_ATTR) == 0) {
            ledState = it->value().as<bool>();
            digitalWrite(48, ledState);
            Serial.println("LED state is set to: ");
            Serial.print(ledState);
        }
    }
    attributesChanged = true;
}

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, &LED_STATE_ATTR, &LED_STATE_ATTR+1);
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, &LED_STATE_ATTR, &LED_STATE_ATTR+1);

// Task CoreIoT
void coreiot_task(void *pvParameters) {
    SystemHandles* handles = (SystemHandles*)pvParameters;

    // Khai báo biến runtime
    const char* TOKEN = handles->sysData.coreiot_token.c_str();
    const char* THINGSBOARD_SERVER = handles->sysData.coreiot_server.c_str();
    uint16_t THINGSBOARD_PORT = handles->sysData.coreiot_port.toInt();
    for (;;) {
        if (WiFi.status() == WL_CONNECTED) {
        if (!tb.connected()) {
            if (tb.connect(handles->sysData.coreiot_server.c_str(), handles->sysData.coreiot_token.c_str(), handles->sysData.coreiot_port.toInt())){
                Serial.println("Successed to connect");
                tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend());
                Serial.println("Failed to subscribe for RPC");
                tb.Shared_Attributes_Subscribe(attributes_callback);
                Serial.println("Failed to subscribe for shared attribute updates");
                tb.Shared_Attributes_Request(attribute_shared_request_callback);
                Serial.println("Failed to request for shared attributes");
            }
        }

        if (attributesChanged) {
            attributesChanged = false;
            tb.sendAttributeData(LED_STATE_ATTR, digitalRead(48));
        }

        if (millis() - previousDataSend > telemetrySendInterval) {
            previousDataSend = millis();
            float temp = 0.0; 
            float humi = 0.0;
            SensorData d = {0, 0, 0};
                
            if (handles->qLcd != NULL) {
                if (xQueuePeek(handles->qLcd, &d, 0) == pdTRUE) {
                    temp = d.temperature;
                    humi = d.humidity;
                }
            }
            tb.sendTelemetryData("temperature", temp);
            tb.sendTelemetryData("humidity", humi);
        }
      tb.loop();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}