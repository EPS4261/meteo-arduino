#define ETL_NO_STL

#include <Arduino.h>

#include "Embedded_Template_Library.h"

//#define ARDUINO_AVR_MEGA2560

#include "meteo.h"
#include "wind_double.h"
#include "/home/pavel/Programs/Arduino/libraries/DHT_sensor_library/DHT.h"
//#include <DHT.h>

#define LOOP_DELAY 1000

class MyMeteo : public MeteoStation {
private:
    DHT dht;
    WindSensorDouble windSensor;

public:    
    using pin = uint8_t;
    MyMeteo(pin dht22pin, pin trig1, pin echo1, pin trig2, pin echo2) : dht(dht22pin, DHT22), windSensor(trig1, echo1, trig2, echo2) {};

    void begin() override {
        dht.begin();
        windSensor.begin();
    }

    void updateHumi() override {
        humi = dht.readHumidity(true);
    }

    void updatePres() override {
        //
    }

    void updateTemp() override {
        temp = dht.readTemperature(true);
    }

    void updateWind() override {
        windSensor.calibrateByTemperature(temp);
        wind = windSensor.readWind();
    }
};

MyMeteo meteoStation(1, 2, 3, 4, 5);

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

