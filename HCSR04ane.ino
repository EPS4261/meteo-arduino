#define ETL_NO_STL

#include <Arduino.h>

#include "Embedded_Template_Library.h"

//#define ARDUINO_AVR_MEGA2560

#include "meteo.h"
#include "wind_double.h"
#include "/home/pavel/Programs/Arduino/libraries/DHT_sensor_library/DHT.h"
//#include <DHT.h>

#define LOOP_DELAY 1000





void setup() {
    Serial.begin(9600);
    while (!Serial) {}
    delay(2000);

    Serial.println("STARTING");
    
    meteoStation.begin();
    Serial.println(meteoStation.getWeatherString().c_str());
}


void loop() {
    meteoStation.update();
    
    Serial.println(meteoStation.getWeatherString().c_str());
    
    delay(LOOP_DELAY);
}

