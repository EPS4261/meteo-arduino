#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "Embedded_Template_Library.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/string_stream.h"

#include "etl/memory.h"


class TemperatureSensor {
public:
    virtual bool begin() = 0;
    virtual float readTemperature() = 0;
    virtual etl::istring getSensorName() = 0;
    virtual ~TemperatureSensor() {}
};

class HumiditySensor {
public:
    virtual bool begin() = 0;
    virtual float readHumidity() = 0;
    virtual etl::istring getSensorName() = 0;
    virtual ~HumiditySensor() {}
};

class PressureSensor {
public:
    virtual bool begin() = 0;
    virtual float readPressure() = 0;
    virtual etl::istring getSensorName() = 0;
    virtual ~PressureSensor() {}
};

class WindSensor {
public:
    virtual bool begin() = 0;
    virtual etl::array<float, 3> readWind() = 0;
    virtual etl::istring getSensorName() = 0;
    virtual ~WindSensor() {}
};



class MeteoStation {
public:
    float getHumidity() const { return humi; }
    float getPressure() const { return pres; }
    float getTemperature() const { return temp; }
    
    float getWindX() const { return wind[0]; }
    float getWindY() const { return wind[1]; }
    float getWindZ() const { return wind[2]; }
    etl::array<float, 3> getWind() const { return wind; }

    etl::istring getWeatherString() const;
    
private:
    float humi = NAN;   // humidity (%)
    float pres = NAN;   // pressure (hPa)
    float temp = NAN;  // temperature (Celsius) 
    etl::array<float, 3> wind = {NAN, NAN, NAN}; // wind speed (m/s X,Y,Z)

    HumiditySensor* humiSensor = nullptr;
    PressureSensor* presSensor = nullptr;
    TemperatureSensor* tempSensor = nullptr;
    WindSensor* windSensor = nullptr;
};


etl::istring MeteoStation::getWeatherString() const {
    etl::string<100> wthr;
    etl::string_stream stream(wthr);
    stream << etl::setw(10) << etl::setprecision(2) << getHumidity();
    stream << etl::setw(10) << etl::setprecision(2) << getPressure();
    stream << etl::setw(10) << etl::setprecision(2) << getTemperature();
    stream << etl::setw(10) << etl::setprecision(2) << getWindX();
    stream << etl::setw(10) << etl::setprecision(2) << getWindY();
    stream << etl::setw(10) << etl::setprecision(2) << getWindZ();
    return wthr;
}

