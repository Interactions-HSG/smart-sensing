#ifndef GPIO_H_
#define GPIO_H_

#include <stdio.h>
#include <unistd.h>

extern uint8_t log_iterator;

/**************************************************************************************************
  GPIOs.
**************************************************************************************************/
extern struct gpio_dt_spec out0;
extern struct gpio_dt_spec out1;
extern struct gpio_dt_spec out2;

/**************************************************************************************************
  Initializes GPIOs as outputs.
**************************************************************************************************/
int gpio_output_init(void);
void gpio_output_set(int pin, int state);

#endif /* GPIO_H_ */
