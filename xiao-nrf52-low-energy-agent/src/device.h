#ifndef DEVICE_H_
#define DEVICE_H_
#include <Arduino.h>

#define MEAS_SENSOR 0
#define MEAS_BATLEVEL 1
#define ADDR_BROADCAST 0xffff
#define DATA_KEY_MEASUREMENT 0x0001

void sleep(uint32_t duration_ms);
void send_data(uint16_t destination, uint16_t key, uint16_t value);
uint32_t get_measurement(uint8_t quantity);
void log(String message);

#endif /* DEVICE_H_ */