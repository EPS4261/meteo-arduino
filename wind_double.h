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

class WindSensorDouble {
public:
    WindSensorDouble(pin trig11, pin echo11, pin trig12, pin echo12) {
        this->axes.push_back({{ TrigEcho{trig11, echo11}, TrigEcho{trig12, echo12} }});
        calibrateByTemperature(TEMP_DEFAULT);
        
        Serial.print("Speed of sound\t");
        Serial.println(speed_of_sound);
    }
    
    WindSensorDouble(pin trig11, pin echo11, pin trig12, pin echo12, pin trig21, pin echo21, pin trig22, pin echo22) {
        axes.reserve(2);
        this->axes.push_back({{ TrigEcho{trig11, echo11}, TrigEcho{trig12, echo12} }});
        this->axes.push_back({{ TrigEcho{trig21, echo21}, TrigEcho{trig22, echo22} }});
        calibrateByTemperature(TEMP_DEFAULT);
    }
    
    void begin();
    void calibrateByTemperature(float temp);
    void calibrateBySpeedOfSound(float speed_of_sound_new);
    Vector3 readWind();

private:
    void pulse(uint8_t axis_index) const;
    int getPulse(uint8_t axis_index, bool sensor_index) const;
    
    struct TrigEcho {
        pin trig;
        pin echo;
        constexpr TrigEcho(pin trig, pin echo) : trig(trig), echo(echo){};
    };

    etl::vector<etl::array<TrigEcho, 2>, 3> axes;

    float speed_of_sound; // speed of sound (m/s)
};



void WindSensorDouble::pulse(uint8_t axis_index) const {
    const auto& axis = axes.at(axis_index);

    digitalWrite(axis[0].trig, LOW);
    digitalWrite(axis[1].trig, LOW);
    delayMicroseconds(PULSE_DELAY_L);
    digitalWrite(axis[0].trig, HIGH);
    digitalWrite(axis[1].trig, HIGH);
    delayMicroseconds(PULSE_DELAY_H);
    digitalWrite(axis[0].trig, LOW);
    digitalWrite(axis[1].trig, LOW);
}

int WindSensorDouble::getPulse(uint8_t axis_index, bool sensor_index) const {
    const auto& echo = axes.at(axis_index).at(sensor_index).echo;
    return pulseIn(echo, HIGH);
}

void WindSensorDouble::begin() {
    for (auto& axis : axes) {
        pinMode(axis[0].trig, OUTPUT);
        pinMode(axis[0].echo, INPUT);
        pinMode(axis[1].trig, OUTPUT);
        pinMode(axis[1].echo, INPUT);
    }
}

void WindSensorDouble::calibrateByTemperature(float temp) {
    if (!isnan(temp)) {
        speed_of_sound = 331.3 + 0.606 * temp;
    }
} 

void WindSensorDouble::calibrateBySpeedOfSound(float speed_of_sound_new) {
    speed_of_sound = speed_of_sound_new;
}



Vector3 WindSensorDouble::readWind() {
    Vector3 result{};
    for (int i = 0; i < axes.size(); i++) {
        etl::array<uint32_t, BURST_SIZE> pulses1;
        etl::array<uint32_t, BURST_SIZE> pulses2;
        for(int j = 0; j < BURST_SIZE; j++) {
            pulse(i);
            pulses1[j] = getPulse(i, 0);
            Serial.print(pulses1[j]);
            delay(PULSE_DELAY_WAIT);
            pulse(i);
            pulses2[j] = getPulse(i, 1);
            Serial.print('\t');
            Serial.println(pulses2[j]);
            delay(PULSE_DELAY_WAIT);
        }

        etl::sort(pulses1.begin(), pulses1.end());
        float dt1 = (BURST_SIZE % 2 == 1) ? pulses1[BURST_SIZE/2] : (pulses1[BURST_SIZE/2 - 1] + pulses1[BURST_SIZE/2]) / 2.0;
        //Serial.println(dt1);
        
        etl::sort(pulses2.begin(), pulses2.end());
        float dt2 = (BURST_SIZE % 2 == 1) ? pulses2[BURST_SIZE/2] : (pulses2[BURST_SIZE/2 - 1] + pulses2[BURST_SIZE/2]) / 2.0;
        //Serial.println(dt2);

        result[i] = 2.0 * dist / (dt1 + dt2) * 1e6 - speed_of_sound;
    }
    return result;
}

