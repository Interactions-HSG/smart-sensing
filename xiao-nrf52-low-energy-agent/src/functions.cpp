#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

//#include <iostream>
#include <Arduino.h>
//#include <M5Stack.h>
#include "functions.h"

#define PIN_DEBUG_SIGNAL 10
#define PIN_BATTERY_VOLTAGE A0
#define PIN_SENSOR A1



bool action_read_sensor()
{
  analogRead(PIN_SENSOR);

  return true;
}

bool action_transmit_data()
{
  for(int i=0;i<2;i++)
    delay(1);
  return true;
}

bool action_sleep()
{

  return true;
}

bool update_energy_buffer_low(bool var){
  uint batvol = analogRead(PIN_BATTERY_VOLTAGE);
  return batvol < (0xFFFFFFFF / 2);
}

bool update_energy_buffer_high(bool var){
  uint batvol = analogRead(PIN_BATTERY_VOLTAGE);
  return batvol >= (0xFFFFFFFF / 2);;
}

#endif /* FUNCTIONS_H_ */