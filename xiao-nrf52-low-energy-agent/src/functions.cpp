#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

//#include <iostream>
#include <Arduino.h>
#include "functions.h"
#include "device.h"


#define BATLEVEL_LOW_THRESHOLD 50
#define SLEEP_DURATION_MS 5000

static uint32_t last_measurement = 0;


bool action_read_sensor()
{
  last_measurement = get_measurement(MEAS_SENSOR);
  return true;
}

bool action_transmit_data()
{
  send_data(ADDR_BROADCAST, DATA_KEY_MEASUREMENT, last_measurement);
  return true;
}

bool action_sleep()
{
  sleep(SLEEP_DURATION_MS);
  return true;
}

bool update_energy_buffer_low(bool var){
  uint batvol = get_measurement(MEAS_BATLEVEL);
  return batvol < BATLEVEL_LOW_THRESHOLD;
}

bool update_energy_buffer_high(bool var){
  uint batvol = get_measurement(MEAS_BATLEVEL);
  return batvol >= BATLEVEL_LOW_THRESHOLD;
}

#endif /* FUNCTIONS_H_ */