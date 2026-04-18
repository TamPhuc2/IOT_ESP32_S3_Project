#include "coreiot.h"

// Các hằng số kết nối CoreIoT
constexpr char TOKEN[] = "7d8thcolc3gb7jirlyp1";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;

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
    digitalWrite(48, newState);
    attributesChanged = true;
    return RPC_Response("setLedSwitchValue", setLedSwitchState);
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
        }
    }
    attributesChanged = true;
}

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, &LED_STATE_ATTR, &LED_STATE_ATTR+1);
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, &LED_STATE_ATTR, &LED_STATE_ATTR+1);

// Task CoreIoT
void coreiot_task(void *pvParameters) {
    SystemHandles* handles = (SystemHandles*)pvParameters;
    for (;;) {
        if (WiFi.status() == WL_CONNECTED) {
        if (!tb.connected()) {
            if (tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
                tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend());
                tb.Shared_Attributes_Subscribe(attributes_callback);
                tb.Shared_Attributes_Request(attribute_shared_request_callback);
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