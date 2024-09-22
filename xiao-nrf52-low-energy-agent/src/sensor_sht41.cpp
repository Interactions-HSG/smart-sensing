#include "sensor_sht41.h"
#include "Adafruit_SHT4x.h"

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

bool init_shtx(){
    sht4.setPrecision(SHT4X_LOW_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
    return sht4.begin();
}

sht4x_reading read_shtx(){
    sensors_event_t humidity, temp;
    sht4.getEvent(&humidity, &temp);
    sht4x_reading reading;
    reading.temperature = temp.temperature;
    reading.humidity = humidity.relative_humidity;
    return reading;
}

void reset_shtx(){
    sht4.reset();
}