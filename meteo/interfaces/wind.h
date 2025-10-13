#pragma once

#include <Arduino.h>

#include "Embedded_Template_Library.h"
#include "etl/array.h"
#include "etl/vector.h"
#include "etl/algorithm.h"
#include "etl/tuple.h"


#define TEMP_DEFAULT 15
#define dist 0.70

#ifndef PULSE_DELAY_L
    #define PULSE_DELAY_L 30 // microseconds
#endif
#ifndef PULSE_DELAY_H
    #define PULSE_DELAY_H 70 // microseconds
#endif
#ifndef PULSE_DELAY_WAIT
    #define PULSE_DELAY_WAIT 50 // milliseconds
#endif
#ifndef BURST_SIZE
    #define BURST_SIZE 20
#endif

using pin = uint8_t;
using Vector3 = etl::array<float, 3>;

class WindSensor {
public:
    WindSensor(pin trig, pin echo) {
        this->axes.emplace_back(trig, echo);
        calibrateByTemperature(TEMP_DEFAULT);
    }
    WindSensor(pin trig1, pin echo1, pin trig2, pin echo2) {
        this->axes.emplace_back(trig1, echo1);
        this->axes.emplace_back(trig2, echo2);
        calibrateByTemperature(TEMP_DEFAULT);
    }
    WindSensor(pin trig1, pin echo1, pin trig2, pin echo2, pin trig3, pin echo3) {
        this->axes.emplace_back(trig1, echo1);
        this->axes.emplace_back(trig2, echo2);
        this->axes.emplace_back(trig3, echo3);
        calibrateByTemperature(TEMP_DEFAULT);
    }
    
    void begin();
    void calibrateByTemperature(float temp);
    void calibrateBySpeedOfSound(float speed_of_sound_new);
    Vector3 readWind();

private:
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



void WindSensor::pulse(uint8_t axis_index) const {
    const auto& trig = axes.at(axis_index).trig;

    digitalWrite(trig, 0);
    delayMicroseconds(PULSE_DELAY_L);
    digitalWrite(trig, 1);
    delayMicroseconds(PULSE_DELAY_H);
    digitalWrite(trig, 0);
}

int WindSensor::getPulse(uint8_t axis_index) const {
    const auto& echo = axes.at(axis_index).echo;
    return pulseIn(echo, HIGH);
}

void WindSensor::begin() {
    for (auto& axis : axes) {
        pinMode(axis.trig, OUTPUT);
        pinMode(axis.echo, INPUT);
    }
}

void WindSensor::calibrateByTemperature(float temp) {
    if (!isnan(temp)) {
        speed_of_sound = 331.3 + 0.606 * temp;
    }
} 

void WindSensor::calibrateBySpeedOfSound(float speed_of_sound_new) {
    speed_of_sound = speed_of_sound_new;
}



Vector3 WindSensor::readWind() {
    Vector3 result{};
    for (int i = 0; i < axes.size(); i++) {
        etl::array<uint32_t, BURST_SIZE> pulses;
        for(int j = 0; j < BURST_SIZE; j++) {
            pulse(i);
            pulses[j] = getPulse(i);
            delay(PULSE_DELAY_WAIT);
        }

        etl::sort(pulses.begin(), pulses.end());
        uint32_t dt = (BURST_SIZE % 2 == 1) ? pulses[BURST_SIZE/2] : (pulses[BURST_SIZE/2 - 1] + pulses[BURST_SIZE/2]) / 2;

        result[i] = dist / dt * 1e6 - speed_of_sound;
    }

    return result;
}

