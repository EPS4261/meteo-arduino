#pragma once

#include "Embedded_Template_Library.h"
#include "etl/array.h"
#include "etl/algorithm.h"

#include "meteo.h"

#define TEMP_DEFAULT 15
#define dist 0.685

#ifndef PULSE_DELAY_L
    #define PULSE_DELAY_L 10 // microseconds
#endif
#ifndef PULSE_DELAY_H
    #define PULSE_DELAY_H 10 // microseconds
#endif
#ifndef PULSE_DELAY_WAIT
    #define PULSE_DELAY_WAIT 50 // milliseconds
#endif

using pin = uint8_t;
using Vector3 = etl::array<float, 3>;

template<size_t BURST_SIZE = 40>
class HCSR04WindSensor : public WindSensor {
    static_assert(BURST_SIZE > 0, "BURST_SIZE must be greater than 0");
public:
    HCSR04WindSensor(pin trig, pin echo) {
        this->axes.emplace_back(trig, echo);
        calibrateByTemperature(TEMP_DEFAULT);
    }
    HCSR04WindSensor(pin trig1, pin echo1, pin trig2, pin echo2) {
        this->axes.emplace_back(trig1, echo1);
        this->axes.emplace_back(trig2, echo2);
        calibrateByTemperature(TEMP_DEFAULT);
    }
    HCSR04WindSensor(pin trig1, pin echo1, pin trig2, pin echo2, pin trig3, pin echo3) {
        this->axes.emplace_back(trig1, echo1);
        this->axes.emplace_back(trig2, echo2);
        this->axes.emplace_back(trig3, echo3);
        calibrateByTemperature(TEMP_DEFAULT);
    }

    HCSR04WindSensor(TemperatureSensor* tsensor, pin trig, pin echo) : tsensor(tsensor) {
        this->axes.emplace_back(trig, echo);
        calibrateByTemperature(TEMP_DEFAULT);
        //calibrateByTemperature(tsensor->readTemperature());
    }
    HCSR04WindSensor(TemperatureSensor* tsensor, pin trig1, pin echo1, pin trig2, pin echo2) : tsensor(tsensor) {
        this->axes.emplace_back(trig1, echo1);
        this->axes.emplace_back(trig2, echo2);
        calibrateByTemperature(TEMP_DEFAULT);
        //calibrateByTemperature(tsensor->readTemperature());
    }
    HCSR04WindSensor(TemperatureSensor* tsensor, pin trig1, pin echo1, pin trig2, pin echo2, pin trig3, pin echo3) : tsensor(tsensor) {
        this->axes.emplace_back(trig1, echo1);
        this->axes.emplace_back(trig2, echo2);
        this->axes.emplace_back(trig3, echo3);
        calibrateByTemperature(TEMP_DEFAULT);
        //calibrateByTemperature(tsensor->readTemperature());
    }

    
    void begin();
    void calibrateByTemperature(float temp);
    void calibrateBySpeedOfSound(float speed_of_sound_new);
    Vector3 readWind();

private:
    TemperatureSensor* tsensor = nullptr;
    bool begun = false;

    void pulse(uint8_t axis_index) const;
    int getPulse(uint8_t axis_index) const;
    
    struct TrigEcho {
        pin trig;
        pin echo;
        TrigEcho(pin trig, pin echo) : trig(trig), echo(echo){};
    };

    etl::vector<TrigEcho, 3> axes;

    float speed_of_sound; // speed of sound (m/s)
};



template<size_t BURST_SIZE>
void HCSR04WindSensor<BURST_SIZE>::pulse(uint8_t axis_index) const {
    const auto& trig = axes[axis_index].trig;

    digitalWrite(trig, 0);
    delayMicroseconds(PULSE_DELAY_L);
    digitalWrite(trig, 1);
    delayMicroseconds(PULSE_DELAY_H);
    digitalWrite(trig, 0);
}

template<size_t BURST_SIZE>
int HCSR04WindSensor<BURST_SIZE>::getPulse(uint8_t axis_index) const {
    const auto& echo = axes[axis_index].echo;
    return pulseIn(echo, HIGH);
}

template<size_t BURST_SIZE>
void HCSR04WindSensor<BURST_SIZE>::begin() {
    if(begun) return;
    begun = true;
    for (auto& axis : axes) {
        pinMode(axis.trig, OUTPUT);
        pinMode(axis.echo, INPUT);
    }
}

template<size_t BURST_SIZE>
void HCSR04WindSensor<BURST_SIZE>::calibrateByTemperature(float temp) {
    if (!isnan(temp)) {
        speed_of_sound = 331.3 + 0.606 * temp;
    }
} 

template<size_t BURST_SIZE>
void HCSR04WindSensor<BURST_SIZE>::calibrateBySpeedOfSound(float speed_of_sound_new) {
    speed_of_sound = speed_of_sound_new;
}



template<size_t BURST_SIZE>
Vector3 HCSR04WindSensor<BURST_SIZE>::readWind() {
    Vector3 result{};
    if(tsensor != nullptr)
        calibrateByTemperature(tsensor->readTemperature());

    const uint8_t axes_size = axes.size();
    for (uint8_t i = 0; i < axes_size; i++) {
        etl::array<uint32_t, BURST_SIZE> pulses;
        for(uint8_t j = 0; j < BURST_SIZE; j++) {
            pulse(i);
            pulses[j] = getPulse(i);
            delay(PULSE_DELAY_WAIT);
        }

        const uint8_t median_pos = BURST_SIZE / 2;
        etl::nth_element(pulses.begin(), pulses.begin() + median_pos, pulses.end());
        uint32_t dt = (BURST_SIZE % 2 == 1) ? pulses[median_pos] : (pulses[median_pos - 1] + pulses[median_pos]) / 2;

        result[i] = dist / dt * 1e6 - speed_of_sound;
    }

    return result;
}

