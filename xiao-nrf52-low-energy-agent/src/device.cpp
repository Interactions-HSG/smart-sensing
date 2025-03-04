
#include "device.h"
#include "Arduino.h"
#include "ble_uart_peripheral.h"
#include "sensor_sht41.h"

#define PIN_DEBUG_SIGNAL 10
#define PIN_BATTERY_VOLTAGE A0
#define PIN_SENSOR A1

void sleep(uint32_t duration_ms){

}

void send_data(uint16_t destination, uint16_t key, uint16_t value)
{
    start_adv(key, value, 0, 30, 60);
}

uint32_t get_measurement(uint8_t quantity){
    if(quantity == MEAS_SENSOR){
        sht4x_reading meas = read_shtx();
        //Serial.println(meas.temperature);
        //Serial.println(meas.humidity);
        return (uint32_t)meas.temperature;
    }
    else{
        return (uint32_t)(analogRead(PIN_BATTERY_VOLTAGE) / 10);
    }
}

void log(String message){
    #ifdef DEBUG_LOG
    Serial.println(message);
    #endif
}

