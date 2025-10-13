#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "../Embedded_Template_Library.h"
#include "../etl/array.h"
#include "../etl/string.h"
#include "../etl/string_stream.h"

#include "../etl/memory.h"


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

template<short num_axis = 3>
class WindSensor {
public:
    virtual bool begin() = 0;
    virtual etl::array<float, num_axis> readWind() = 0;
    virtual etl::istring getSensorName() = 0;
    virtual ~WindSensor() {}
};



class MeteoStation {
public:
    constexpr float getHumidity() const { return humi; }
    constexpr float getPressure() const { return pres; }
    constexpr float getTemperature() const { return temp; }
    
    constexpr float getWindX() const { return wind[0]; }
    constexpr float getWindY() const { return wind[1]; }
    constexpr float getWindZ() const { return wind[2]; }
    constexpr etl::array<float, 3> getWind() const { return wind; }

    etl::string<61> getWeatherString() const;
    
protected:
    float humi = 50;   // humidity (%)
    float pres = 761;   // pressure (mmHg)
    float temp = 20;  // temperature (Celsius) 
    etl::array<float, 3> wind = {}; // wind speed (m/s X,Y,Z)
};


etl::string<61> MeteoStation::getWeatherString() const {
    etl::string<61> wthr;
    etl::string_stream stream(wthr);
    stream << etl::setw(10) << etl::setprecision(2) << getHumidity();
    stream << etl::setw(10) << etl::setprecision(2) << getPressure();
    stream << etl::setw(10) << etl::setprecision(2) << getTemperature();
    stream << etl::setw(10) << etl::setprecision(2) << getWindX();
    stream << etl::setw(10) << etl::setprecision(2) << getWindY();
    stream << etl::setw(10) << etl::setprecision(2) << getWindZ();
    return wthr;
}

