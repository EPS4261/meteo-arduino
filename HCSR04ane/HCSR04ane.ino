#include <Arduino.h>

#define ETL_NO_STL
#include "src/Embedded_Template_Library.h"

#include "src/meteo.h"
#include "src/dht_interface.h"
#include "src/wind.h"

#include <DHT.h>

#define LOOP_DELAY 1000


DHT11Sensor dht11(7);
MeteoStation meteo;
HCSR04WindSensor windSensor(&dht11, 10, 8);

void setup() {
    Serial.begin(9600);
    while(!Serial) {}
    delay(5000);

    Serial.println("Setup...");


    meteo.setTemperatureSensor(&dht11);
    meteo.setHumiditySensor(&dht11);
    meteo.setWindSensor(&windSensor);
    Serial.println(11);

    //meteo.begin();
    dht11.begin();
    windSensor.begin();
    
    Serial.println(22);
    Serial.println(meteo.getWeatherString().c_str());
    Serial.println("Setup complete.");
}


void loop() {
    meteo.update();
    
    Serial.println(meteo.getWeatherString().c_str());
    
    delay(LOOP_DELAY);
}

