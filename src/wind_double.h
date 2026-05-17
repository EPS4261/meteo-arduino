#pragma once

#include "Embedded_Template_Library.h"
#include "etl/array.h"
#include "etl/algorithm.h"

#include "meteo.h"

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
class HCSR04WindSensorDouble : public WindSensor {
    static_assert(BURST_SIZE > 0, "BURST_SIZE must be greater than 0");
public:
    HCSR04WindSensorDouble(pin trig1_fwd, pin echo1_fwd, pin trig1_bwd, pin echo1_bwd) {
        this->axes.emplace_back(trig1_fwd, echo1_fwd, trig1_bwd, echo1_bwd);
    }
    HCSR04WindSensorDouble(pin trig1_fwd, pin echo1_fwd, pin trig1_bwd, pin echo1_bwd,
                            pin trig2_fwd, pin echo2_fwd, pin trig2_bwd, pin echo2_bwd) {
        this->axes.emplace_back(trig1_fwd, echo1_fwd, trig1_bwd, echo1_bwd);
        this->axes.emplace_back(trig2_fwd, echo2_fwd, trig2_bwd, echo2_bwd);
    }
    HCSR04WindSensorDouble(pin trig1_fwd, pin echo1_fwd, pin trig1_bwd, pin echo1_bwd,
                            pin trig2_fwd, pin echo2_fwd, pin trig2_bwd, pin echo2_bwd,
                            pin trig3_fwd, pin echo3_fwd, pin trig3_bwd, pin echo3_bwd) {
        this->axes.emplace_back(trig1_fwd, echo1_fwd, trig1_bwd, echo1_bwd);
        this->axes.emplace_back(trig2_fwd, echo2_fwd, trig2_bwd, echo2_bwd);
        this->axes.emplace_back(trig3_fwd, echo3_fwd, trig3_bwd, echo3_bwd);
    }

    void begin();
    Vector3 readWind();

private:
    void pulse(uint8_t axis_index, uint8_t direction) const;
    int getPulse(uint8_t axis_index, uint8_t direction) const;
    
    struct TrigEchoPair {
        pin trig_fwd, echo_fwd;
        pin trig_bwd, echo_bwd;
        TrigEchoPair(pin tf, pin ef, pin tb, pin eb) 
            : trig_fwd(tf), echo_fwd(ef), trig_bwd(tb), echo_bwd(eb) {}
    };

    etl::vector<TrigEchoPair, 3> axes;
};



template<size_t BURST_SIZE>
void HCSR04WindSensorDouble<BURST_SIZE>::pulse(uint8_t axis_index, uint8_t direction) const {
    pin trig = (direction == 0) ? axes[axis_index].trig_fwd : axes[axis_index].trig_bwd;

    digitalWrite(trig, 0);
    delayMicroseconds(PULSE_DELAY_L);
    digitalWrite(trig, 1);
    delayMicroseconds(PULSE_DELAY_H);
    digitalWrite(trig, 0);
}

template<size_t BURST_SIZE>
int HCSR04WindSensorDouble<BURST_SIZE>::getPulse(uint8_t axis_index, uint8_t direction) const {
    pin echo = (direction == 0) ? axes[axis_index].echo_fwd : axes[axis_index].echo_bwd;
    return pulseIn(echo, HIGH);
}

template<size_t BURST_SIZE>
void HCSR04WindSensorDouble<BURST_SIZE>::begin() {
    if(begun) return;
    begun = true;
    for (auto& axis : axes) {
        pinMode(axis.trig_fwd, OUTPUT);
        pinMode(axis.echo_fwd, INPUT);
        pinMode(axis.trig_bwd, OUTPUT);
        pinMode(axis.echo_bwd, INPUT);
    }
}

template<size_t BURST_SIZE>
Vector3 HCSR04WindSensorDouble<BURST_SIZE>::readWind() {
    Vector3 result{};
    const uint8_t axes_size = axes.size();
    
    for (uint8_t i = 0; i < axes_size; i++) {
        etl::array<uint32_t, BURST_SIZE> pulses_fwd;
        for(uint8_t j = 0; j < BURST_SIZE; j++) {
            pulse(i, 0);
            pulses_fwd[j] = getPulse(i, 0);
            delay(PULSE_DELAY_WAIT);
        }

        etl::array<uint32_t, BURST_SIZE> pulses_bwd;
        for(uint8_t j = 0; j < BURST_SIZE; j++) {
            pulse(i, 1);
            pulses_bwd[j] = getPulse(i, 1);
            delay(PULSE_DELAY_WAIT);
        }

        const uint8_t median_pos = BURST_SIZE / 2;
        etl::nth_element(pulses_fwd.begin(), pulses_fwd.begin() + median_pos, pulses_fwd.end());
        uint32_t dt_fwd = (BURST_SIZE % 2 == 1) ? pulses_fwd[median_pos] 
                                                  : (pulses_fwd[median_pos - 1] + pulses_fwd[median_pos]) / 2;

        etl::nth_element(pulses_bwd.begin(), pulses_bwd.begin() + median_pos, pulses_bwd.end());
        uint32_t dt_bwd = (BURST_SIZE % 2 == 1) ? pulses_bwd[median_pos] 
                                                  : (pulses_bwd[median_pos - 1] + pulses_bwd[median_pos]) / 2;

        result[i] = (dist / dt_fwd - dist / dt_bwd) * 1e6 / 2.0;
    }

    return result;
}
