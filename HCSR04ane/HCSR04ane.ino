#include <Arduino.h>

#define ETL_NO_STL
#include "src/Embedded_Template_Library.h"

#include "src/meteo.h"
#include "src/dht22.h"

//#include "/home/pavel/Programs/Arduino/libraries/DHT_sensor_library/DHT.h"
//#include <DHT.h>

#define LOOP_DELAY 1000


DHT22Sensor dht22(1);
MeteoStation meteo;

void setup() {
    Serial.begin(9600);
    while (!Serial) {}
    delay(2000);

    Serial.println("STARTING");
    
    meteo.setTemperatureSensor(&dht22);
    meteo.setHumiditySensor(&dht22);

    meteo.begin();
    
    Serial.println(meteo.getWeatherString().c_str());
}


void loop() {
    meteo.update();
    
    Serial.println(meteo.getWeatherString().c_str());
    
    delay(LOOP_DELAY);
}

