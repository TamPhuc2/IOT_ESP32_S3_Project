#include <Temp_Hum_Sensor.h>
DHT20 dht20;

void Temp_Hum_Sensor(void *pvParameters){

    Wire.begin(11,12);
    Serial.begin(115200);
    dht20.begin();

    while (1){
        // if(xSemaphoreTake(i2cSemaphore, portMAX_DELAY)){ 
            dht20.read();
            glob_temperature = dht20.getTemperature();
            glob_humidity = dht20.getHumidity();

            if (isnan(glob_temperature) || isnan(glob_humidity)){
                Serial.println("Failed to read from DHT sensor."); 
               return;  
            }

            Serial.print("Temperature: ");
            Serial.print(glob_temperature);
            Serial.write(176);
            Serial.println("C");
            Serial.print("Humidity: ");
            Serial.print(glob_humidity);
            Serial.println("%");
            // xSemaphoreGive(i2cSemaphore);
        // }
        vTaskDelay(5000);
    }
    
}