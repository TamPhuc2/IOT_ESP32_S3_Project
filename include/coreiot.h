#ifndef __COREIOT_H__
#define __COREIOT_H__

#include <Arduino.h>
#include <WiFi.h>
#include "global.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino_MQTT_Client.h>
#include <WiFiClient.h>
#include <ThingsBoard.h>

// Khai báo task CoreIOT
void coreiot_task(void *pvParameters);

// Khai báo các callback RPC và shared attributes
RPC_Response setLedSwitchState(const RPC_Data &data);
void processSharedAttributes(const Shared_Attribute_Data &data);

#endif