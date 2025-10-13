#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "Embedded_Template_Library.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/string_stream.h"


using pin = uint8_t;
using Vector3 = etl::array<float, 3>;


class MeteoStation {
public:
    virtual void begin() {};
    
    virtual void updateHumi() {};
    virtual void updatePres() {};
    virtual void updateTemp() {};
    virtual void updateWind() {};
    
    virtual void onUpdate() {};
    
    void update() {
        onUpdate();
        updateHumi();
        updatePres();
        updateTemp();
        updateWind();
    }

    constexpr float getHumidity() const { return humi; }
    constexpr float getPressure() const { return pres; }
    constexpr float getTemperature() const { return temp; }
    
    constexpr float getWindX() const { return wind[0]; }
    constexpr float getWindY() const { return wind[1]; }
    constexpr float getWindZ() const { return wind[2]; }
    constexpr Vector3 getWind() const { return wind; }

    etl::string<61> getWeatherString() const;
    
protected:
    float humi = 50;   // humidity (%)
    float pres = 761;   // pressure (mmHg)
    float temp = 20;  // temperature (Celsius) 
    Vector3 wind = {}; // wind speed (m/s X,Y,Z)
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

