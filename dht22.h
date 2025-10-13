//#include <DHT.h>
#include "/home/pavel/Programs/Arduino/libraries/DHT_sensor_library/DHT.h"
#include "meteo.h"

class DHT22Sensor : public TemperatureSensor, public HumiditySensor {
private:
    DHT dht;
    uint8_t pin;
    
public:
    DHT22Sensor(uint8_t pin) : dht(pin, DHT22), pin(pin) {}
    
    bool begin() override {
        dht.begin();
        return true;
    }
    
    float readTemperature() override {
        return dht.readTemperature();
    }
    
    float readHumidity() override {
        return dht.readHumidity();
    }
    
    etl::istring getSensorName() override {
        return etl::make_string("DHT22");
    }
};
