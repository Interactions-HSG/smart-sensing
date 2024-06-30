#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gpio, CONFIG_GPIO_LOG_LEVEL);

uint8_t log_iterator = 0;

/**************************************************************************************************
  GPIOs.
**************************************************************************************************/
struct gpio_dt_spec out0 = GPIO_DT_SPEC_GET(DT_NODELABEL(outgpio0), gpios);
struct gpio_dt_spec out1 = GPIO_DT_SPEC_GET(DT_NODELABEL(outgpio1), gpios);
struct gpio_dt_spec out2 = GPIO_DT_SPEC_GET(DT_NODELABEL(outgpio2), gpios);


/**************************************************************************************************
  Functions.
**************************************************************************************************/

/**************************************************************************************************
  Initializes GPIOs as outputs.
**************************************************************************************************/
int gpio_output_init(void)
{
	int err;

	if (!device_is_ready(out0.port) || !device_is_ready(out1.port) || !device_is_ready(out2.port)) {
		LOG_ERR("Error: Output GPIOs not ready!");
		return 1;
	}
	
	err = gpio_pin_configure_dt(&out0, GPIO_OUTPUT_LOW);
	err = gpio_pin_configure_dt(&out1, GPIO_OUTPUT_LOW);
	err = gpio_pin_configure_dt(&out2, GPIO_OUTPUT_LOW);

	if (err) {
		LOG_ERR("Cannot init GPIOs, (error: %d)", err);
		return err;
	}

	return 0;
}

void gpio_output_set(int pin, int state)
{
	 gpio_pin_set(out0.port, pin, state);
}

