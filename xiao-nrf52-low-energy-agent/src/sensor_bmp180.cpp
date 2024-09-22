#include "sensor_bmp180.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

bool init_bmp(){
    return bmp.begin();
}

bmp_reading read_bmp(){
    sensors_event_t event;
    bmp.getEvent(&event);
    bmp_reading reading;
    reading.pressure = event.pressure;
    float temperature;
    bmp.getTemperature(&temperature);
    reading.temperature = temperature;
    return reading;
}

void reset_bmp(){
    
}