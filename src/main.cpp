#include <Arduino.h>
#include <mainserver.h>
#include <Temp_Hum_Sensor.h>

void TaskLEDControl(void *pvParameters) {
  pinMode(GPIO_NUM_48, OUTPUT); // Initialize LED pin
  int ledState = 0;
  while(1) {
    
    if (ledState == 0) {
      digitalWrite(GPIO_NUM_48, HIGH); // Turn ON LED
    } else {
      digitalWrite(GPIO_NUM_48, LOW); // Turn OFF LED
    }
    ledState = 1 - ledState;
    vTaskDelay(500);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  xTaskCreate(TaskLEDControl, "LED Control", 2048, NULL, 2, NULL);
  xTaskCreate(Temp_Hum_Sensor, "Temp_Hum_Sensor", 2048, NULL, 2, NULL);
  // xTaskCreate(setup_server, "Task Main Server" ,8192  ,NULL  ,2 , NULL);
  xTaskCreate(main_server_task, "Task Main Server" ,8192  ,NULL  ,2 , NULL);
}

void loop() {
  // Serial.println("Hello Custom Board");
  // loop_server();
  delay(1000);
}