/**
 * Thing:53 BDI CoAP Client
 *
 * main.cpp
 */

#include <dk_buttons_and_leds.h>
#include <ram_pwrdn.h>
#include <zephyr/pm/device.h>

#include "main.h"
#include "mas-abstractions/organization/Organization.h"


LOG_MODULE_REGISTER(main, CONFIG_COAP_CLIENT_LOG_LEVEL);

/**************************************************************************************************
  Agent ID string.
**************************************************************************************************/
std::string agent_id = "agent-" + std::to_string(AGENT_ID);

/**************************************************************************************************
  LED defines.
**************************************************************************************************/
#define RED_LED				DK_LED1
#define GREEN_LED			DK_LED2
#define BLUE_LED			DK_LED3


/**************************************************************************************************
  Variables for BDI-reasoner
**************************************************************************************************/
int32_t agent_period = DEFAULT_AGENT_PERIOD;

/**************************************************************************************************
  Global Variables.
**************************************************************************************************/

/**************************************************************************************************
  CoAP Variables.
**************************************************************************************************/

BME688* m_p_bme_sensor;


/**************************************************************************************************
  Thread Declarations.
**************************************************************************************************/

/**************************************************************************************************
  Embedded BDI Agent Thread Declaration.
**************************************************************************************************/
void agent_run_thread_cb(void);
K_THREAD_DEFINE(agent_run_thread_id, STACKSIZE, agent_run_thread_cb, NULL, NULL, NULL, 
					AGENT_PRIORITY, 0, 0);

/**************************************************************************************************
  Measurement Thread Declaration.
**************************************************************************************************/
void sensing_thread_cb(void);
K_THREAD_DEFINE(sensing_thread_id, SENSING_STACKSIZE, sensing_thread_cb, NULL, NULL, NULL, 
					SENSOR_PRIORITY, 0, 0);

/**************************************************************************************************
  Transmission Thread Declaration.
**************************************************************************************************/
void transmission_thread_cb(void);
K_THREAD_DEFINE(transmission_thread_id, TRANSMISSION_STACKSIZE, transmission_thread_cb, NULL, NULL, NULL, 
					TRANSMISSION_PRIORITY, 0, 0);


/**************************************************************************************************
  BDI Callback-Functions.
**************************************************************************************************/


/**************************************************************************************************
  OpenThread Callback-Functions.
**************************************************************************************************/

/**************************************************************************************************
  Toggles MED and SED mode when enabled in build.
**************************************************************************************************/
static void on_mtd_mode_toggle(uint32_t med)
{
#if IS_ENABLED(CONFIG_PM_DEVICE)
	const struct device *cons = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

	if (!device_is_ready(cons)) {
		return;
	}
	dk_set_led_off(BLUE_LED);
	dk_set_led_off(RED_LED);
	dk_set_led_off(GREEN_LED);
	//is_med = med == 1 ? true : false;
	if (med) {
		pm_device_action_run(cons, PM_DEVICE_ACTION_RESUME);
		dk_set_led_on(GREEN_LED);
	} else {
		pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND);
		dk_set_led_on(RED_LED);
	}
#endif
	
}

/**************************************************************************************************
  Button Actions.
**************************************************************************************************/
static bool in_role = false;
static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
	uint32_t buttons = button_state & has_changed;
	if ((buttons & DK_BTN1_MSK) && has_changed==1) {
		dk_set_led_off(BLUE_LED);
		dk_set_led_off(RED_LED);
		dk_set_led_off(GREEN_LED);

		dk_set_led_on(GREEN_LED);
		//LOG_INF("Creating organization entity object %d", 2);
		if(in_role){
			Organization::leaveRole("gr_test_role");
			in_role = false;
		}else{
			Organization::joinRole("gr_test_role");
			in_role = true;
		}
		//Organization::sendMeasurement("gr_test_role", 12.1);
	}else if (buttons & DK_BTN2_MSK) {
		// Nothing

	}
}

/**************************************************************************************************
  Sensor Semaphore.
**************************************************************************************************/
K_SEM_DEFINE(sensor_sem, 0, 1);

/**************************************************************************************************
  Sensing Thread. Lower number -> higher prio
**************************************************************************************************/
void sensing_thread_cb(void)
{
	k_sleep(K_FOREVER);
	/* Run sensing loop */
	while(true){
		/* Sleep as long as no role is assigned */
		//LOG_INF("%i: Sensing Thread...\n", log_iterator++);
		/* Read BME688 enviromental sensor */		

		m_p_bme_sensor->measure();

		/* Release transmission when buffer is full */
		if (m_p_bme_sensor->isBufferFull())
		{ 
			k_sem_give(&sensor_sem);
		}
		/* sleep */
		//k_sleep(1000 /*roles[current_role].getMeasurementPeriod()*/);
	}
}

/**************************************************************************************************
  Transmission Thread.
**************************************************************************************************/
void transmission_thread_cb(void)
{
	k_sleep(K_FOREVER);
	/* Run transmission loop */
	while(true){
		/* Transmit to Server */
		k_sem_take(&sensor_sem, K_FOREVER); //Add timeout to prevent deadlock...
		LOG_INF("%i: Transmit sensor data:\n---> %s\n", log_iterator++, m_p_bme_sensor->getPayload() .c_str());

		/* Reset buffer */
		m_p_bme_sensor->clearBuffer();
		/* sleep */
		k_sleep(K_FOREVER);
	}
}

/**************************************************************************************************
  Embedded BDI Agent Thread.
**************************************************************************************************/
void agent_run_thread_cb(void)
{
	k_sleep(K_FOREVER);
	LOG_INF("%i: Initialize Agent Thread...\n", log_iterator++);
	k_msleep(agent_period);
}

/**************************************************************************************************
  Main Function.
**************************************************************************************************/
int main(void)
{
	LOG_INF("%i: Start Thingy:53 BDI CoAP Client...\n", log_iterator++);

	int ret;

	#if USE_GPIO_OUTPUT
		/* Initialize GPIOs */
		ret = gpio_output_init();
		if (ret) {
			LOG_ERR("Cannot init gpios, (error: %d)", ret);
			return 0;
		}
		gpio_pin_set_dt(&out0, 0);
		gpio_pin_set_dt(&out1, 0);
		gpio_pin_set_dt(&out2, 0);
	#endif /* USE_GPIO_OUTPUT */

	/* Enabled if built as sleepy end device */
	if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
		power_down_unused_ram();
	}

	/* Initialize buttons */
	ret = dk_buttons_init(on_button_changed);
	if (ret) {
		LOG_ERR("Cannot init buttons (error: %d)", ret);
		return 0;
	}

	/* Initialize LEDs */
	ret = dk_leds_init();
	if (ret) {
		LOG_ERR("Cannot init leds, (error: %d)", ret);
		return 0;
	}

	/* Initialize Sensor object*/
	m_p_bme_sensor = new BME688(agent_id);

	LOG_INF("Creating CoAP channel %d",1);
	//LOG_INF("CoAP channel initialized. Response=%d", resp);
	startThread(on_mtd_mode_toggle);
	k_msleep(2000);
	otLinkModeConfig mode = getCurrentMode();
	mode.mRxOnWhenIdle = 1;
	setCurrentMode(mode);
	k_msleep(2000);
	CoapClient::initialize();
	LOG_INF("Creating organization entity object %d", 2);
	Organization::refresh();
}