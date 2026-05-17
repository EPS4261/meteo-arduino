#pragma once

#include <DHT.h>
#include "meteo.h"

class DHT22Sensor : public TemperatureSensor, public HumiditySensor {
private:
    DHT dht;
    bool begun = false;
    
public:
    DHT22Sensor(uint8_t pin) : dht(pin, DHT22), pin(pin) {}
    
    void begin() override {
        if(begun) return;
        begun = true;
        dht.begin();
    }
    
    float readTemperature() override {
        return dht.readTemperature();
    }
    
    float readHumidity() override {
        return dht.readHumidity();
    }
};

class DHT11Sensor : public TemperatureSensor, public HumiditySensor {
private:
    DHT dht;
    bool begun = false;
    
public:
    DHT11Sensor(uint8_t pin) : dht(pin, DHT11), pin(pin) {}
    
    void begin() override {
        if(begun) return;
        begun = true;
        dht.begin();
    }
    
    float readTemperature() override {
        return dht.readTemperature();
    }
    
    float readHumidity() override {
        return dht.readHumidity();
    }
};
