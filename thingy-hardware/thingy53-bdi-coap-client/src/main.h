/**
 * main.h
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <math.h>
#include <numeric>
#include <algorithm>
#include "peripherals/BME688.h"

/**************************************************************************************************
  ZEPHYR include files.
**************************************************************************************************/
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#include <openthread/coap.h>

/**************************************************************************************************
  Task include files.
**************************************************************************************************/
/**************************** Embedded BDI Agent ****************************/
#ifdef USE_EMB_BDI
#include "agent/agent.h"
#include "mas-abstractions/agent/bdi-agent/configuration.h"
#include "mas-abstractions/agent/bdi-agent/functions.h"
#endif
/****************************************************************************/

#include <coap_server_client_interface.h>

extern "C" {
  //#include "peripherals/sensors.h"
  //#include "peripherals/battery.h"
  #include "low_power.h"
  #include "peripherals/gpio.h"
  #include "thread/thread_helper.h"
}

/**************************************************************************************************
  Enable or disable parts of the code.
**************************************************************************************************/
#define LOW_POWER						   0
#define USE_GPIO_OUTPUT					   1

/**************************************************************************************************
  Defines.
**************************************************************************************************/
/* Size of stack area used by each thread */
#define STACKSIZE             			4096
#define TRANSMISSION_STACKSIZE          1024
#define SENSING_STACKSIZE          		1024

/* Scheduling priority used by agent thread */
#define AGENT_PRIORITY           		   7

/* Scheduling priority used by agent thread */
#define SENSOR_PRIORITY          		   7

/* Scheduling priority used by agent thread */
#define TRANSMISSION_PRIORITY          	   6

/**************************************************************************************************
  Agent default properties.
**************************************************************************************************/
#define AGENT_ID                           3
#define REASONING_CYCLES                  50
#define DEFAULT_AGENT_PERIOD           5000
#define DEFAULT_SENSING_PERIOD_FACTOR      1
#define MAX_BUFFER_SIZE                   10

/**************************************************************************************************
  Properties to determine if role can be fullfilled.
**************************************************************************************************/
#define MINIMAL_SENSING_PERIOD             5
#define MINIMAL_TRANSMISSION_PERIOD        5

/**************************************************************************************************
  Constants for .xml parser.
**************************************************************************************************/
#define MAX_GOALS                         10
#define MAX_PROPERTIES                    10
#define MAX_ARGUMENTS                     10
#define MAX_ROLES                          3

/**************************************************************************************************
  Variables for BDI-reasoner.
**************************************************************************************************/
extern bool start_agent;
extern bool org_spec_changed;
extern bool rewards_changed;
extern bool local_capabilities_changed;
extern bool optimal_state;
extern bool role_assigned;
extern bool role_deleted;

extern bool role_available;
extern uint8_t iterations;
extern bool transmission_due;
extern int32_t agent_period;
extern uint8_t sensing_period_factor;
extern uint8_t current_buffer_size;
extern double temperature;
extern double delta_temperature;
extern bool ot_sed;
extern uint8_t battery_capacity;

#endif /* MAIN_H_ */
