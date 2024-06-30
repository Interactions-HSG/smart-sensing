/**
 * Thing:53 BDI CoAP Client
 *
 * main.cpp
 */

#include <dk_buttons_and_leds.h>
#include <ram_pwrdn.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/pm.h>
//#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/poweroff.h>
#include <ram_pwrdn.h>

#include "main.h"
#include "mas-abstractions/organization/Organization.h"
#include "mas-abstractions/agent/ReactiveAgent.h"

#define CONFIG_THINGY_LOW_POWER
//#define CONFIG_THINGY_USE_SENSOR
#if defined(CONFIG_THINGY_LOW_POWER)
#include "low_power.h"
#endif


LOG_MODULE_REGISTER(main, CONFIG_COAP_CLIENT_LOG_LEVEL);

//#define GPIO0_NODE DT_NODELABEL(gpio0)
//#define PIN 9

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
//

/**************************************************************************************************
  Global Variables.
**************************************************************************************************/

/**************************************************************************************************
  CoAP Variables.
**************************************************************************************************/
#ifdef CONFIG_THINGY_USE_SENSOR
BME688* m_p_bme_sensor;
#endif
#ifdef USE_EMB_BDI
	Agent* m_p_sensor_agent;
#else
	ReactiveAgent* m_p_sensor_agent;
#endif
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
//typedef struct otLinkModeConfig
//{
//    bool mRxOnWhenIdle : 1; ///< 1, if the sender has its receiver on when not transmitting. 0, otherwise.
//    bool mDeviceType : 1;   ///< 1, if the sender is an FTD. 0, otherwise.
//    bool mNetworkData : 1;  ///< 1, if the sender requires the full Network Data. 0, otherwise.
//}

static void on_mtd_mode_toggle(/*uint32_t med*/otLinkModeConfig mode)
{
#if IS_ENABLED(CONFIG_PM_DEVICE)
	const struct device *cons = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

	if (!device_is_ready(cons)) {
		return;
	}
	uint32_t rxon = mode.mRxOnWhenIdle;
	uint32_t ftd = mode.mDeviceType;
	dk_set_led_off(BLUE_LED);
	dk_set_led_off(RED_LED);
	dk_set_led_off(GREEN_LED);
	if (rxon || ftd) {
		// pm_device_action_run(cons, PM_DEVICE_ACTION_RESUME);
		dk_set_led_on(RED_LED);
	} else {
		// pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND);
		dk_set_led_on(GREEN_LED);
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

		
		//LOG_INF("Creating organization entity object %d", 2);
		if(in_role){
			Organization::leaveRole("rl-1");
			dk_set_led_on(RED_LED);
			in_role = false;
		}else{
			Organization::joinRole("rl-1", 100);
			dk_set_led_on(GREEN_LED);
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
		#ifdef CONFIG_THINGY_USE_SENSOR
		m_p_bme_sensor->measure();

		/* Release transmission when buffer is full */
		if (m_p_bme_sensor->isBufferFull())
		{ 
			k_sem_give(&sensor_sem);
		}
		#endif
		/* sleep */
		//k_sleep(1000 /*roles[current_role].getMeasurementPeriod()*/);
	}
}

/**************************************************************************************************
  Transmission Thread.
**************************************************************************************************/
void transmission_thread_cb(void)
{
	//k_sleep(K_FOREVER);
	/* Run transmission loop */
	while(true){
		/* Transmit to Server */
		k_sem_take(&sensor_sem, K_FOREVER); //Add timeout to prevent deadlock...
		//LOG_INF("%i: Transmit sensor data:\n---> %s\n", log_iterator++, m_p_bme_sensor->getPayload() .c_str());

		/* Reset buffer */
		#ifdef CONFIG_THINGY_USE_SENSOR
		m_p_bme_sensor->clearBuffer();
		#endif
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
	while(true){
		LOG_INF("%s","Waking agent..");
		dk_set_led_off(BLUE_LED);
		dk_set_led_off(RED_LED);
		dk_set_led_on(GREEN_LED);
		m_p_sensor_agent->run();
		k_msleep(agent_period);
	}
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



	//struct otInstance *openthread = openthread_get_default_instance();
	//struct net_if * net = net_if_get_default();

	/* Enabled if built as sleepy end device */
	if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
		//power_down_unused_ram();
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
	#ifdef CONFIG_THINGY_USE_SENSOR
	m_p_bme_sensor = new BME688(agent_id);
	#endif
	m_p_sensor_agent = new ReactiveAgent();
	/* Initialize battery measurement */
	//ret = battery_measure_enable(true);
	//if (ret) {
	//	LOG_ERR("Failed initialize battery measurement (error: %d)", ret);
	//	return 0;
	//}

	LOG_INF("Creating CoAP channel %d",1);
	//LOG_INF("CoAP channel initialized. Response=%d", resp);
	startThread(on_mtd_mode_toggle);	
	k_sleep(K_MSEC(2000));

	dk_set_led_off(BLUE_LED);
	dk_set_led_off(RED_LED);
	dk_set_led_off(GREEN_LED);

	
	CoapClient::initialize();
	dk_set_led_on(BLUE_LED);
	k_sleep(K_MSEC(2000));
	dk_set_led_off(BLUE_LED);
	dk_set_led_off(RED_LED);
	dk_set_led_off(GREEN_LED);

#if defined(CONFIG_THINGY_LOW_POWER)
	low_power_enable();
#endif

	while(true){
		dk_set_led_on(BLUE_LED);
		k_sleep(K_MSEC(500));
		dk_set_led_off(BLUE_LED);
		dk_set_led_off(RED_LED);
		dk_set_led_off(GREEN_LED);
			
		gpio_output_set(9,1); //Trace to mark energy measurement start	
		Organization::refresh();
		//m_p_sensor_agent->run();S
		k_sleep(K_MSEC(10000));
	}
	
}