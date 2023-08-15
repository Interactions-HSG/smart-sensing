/**
 * Thing:53 BDI CoAP Client
 *
 * main.cpp
 */

#include <dk_buttons_and_leds.h>
#include <ram_pwrdn.h>
#include <zephyr/pm/device.h>

#include "main.h"
#include "Role.h"
#include "mas-abstractions/organization/Organization.h"
#include "communication/CoapChannel.h"


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
  Organizational Specification Buffer and Length.
**************************************************************************************************/
//char *org_spec = new char[3000];
char org_spec_buffer[3000];
char *org_spec = org_spec_buffer;

uint16_t org_spec_lenght = 0;
uint16_t current_org_spec_lenght = 0;

/**************************************************************************************************
  Variables for BDI-reasoner
**************************************************************************************************/
int32_t agent_period = DEFAULT_AGENT_PERIOD;
bool start_agent = false;
bool org_spec_changed = false;
bool rewards_changed = false;
bool local_capabilities_changed = false;
bool optimal_state = true;
bool role_assigned = false;
bool role_deleted = true;
bool ready_to_leave = true;
uint8_t total_number_of_roles = 0;
uint8_t current_role = -1;
uint8_t best_role = -2;
bool low_battery = false;

/**************************************************************************************************
  Global Variables.
**************************************************************************************************/
int battery_level = 1;

/**************************************************************************************************
  CoAP Variables.
**************************************************************************************************/
int multiple_responses = 0;


/*
void start_of_experiment(void){
	LOG_INF("%i: START OF EXPERIMENT", log_iterator++);
	gpio_pin_set_dt(&out0, 1);
	gpio_pin_set_dt(&out1, 1);
	gpio_pin_set_dt(&out2, 1);
	k_busy_wait(2000000);
	gpio_pin_set_dt(&out0, 0);
	gpio_pin_set_dt(&out1, 0);
	gpio_pin_set_dt(&out2, 0);
}

void end_of_experiment(void){
	LOG_INF("%i: END OF EXPERIMENT", log_iterator++);
	while (1){
		gpio_pin_set_dt(&out0, 1);
		gpio_pin_set_dt(&out1, 1);
		gpio_pin_set_dt(&out2, 1);
	}
}
*/

std::vector<Role> roles;


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

/*************************************************************************************************/
/*!
 *  \brief  Gets called if the BDI-reasoner requests to leave the current.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void on_bdi_leave_role()
{
	if (ready_to_leave){
		ready_to_leave = false;
		LOG_INF("%i: BDI Leave role CB\n", log_iterator++);
		/* Delete current role from server */
		coap_client_delete_role(roles[current_role].getRolePlayerUri().c_str());
	}
	return;
}

/*************************************************************************************************/
/*!
 *  \brief  Gets called if the BDI-reasoner requests to enter a new role.
 *
 *  \param  uri_role: URI-path of the newly selected role.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void on_bdi_enter_new_role()
{
	LOG_INF("%i: BDI Enter new role CB\n", log_iterator++);
	best_role = find_best_role();
	if(!role_assigned){
		/* Post the new role to the server */
		coap_client_post_role(roles[best_role].getRoleUri().c_str(), 
						agent_id.c_str(), agent_id.size());			
	}
	return;
}

/*************************************************************************************************/
/*!
 *  \brief  Gets called if the BDI-reasoner requests the new role costs in the organization.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void on_bdi_get_org_costs()
{
	LOG_INF("%i: BDI Get Org Costs CB\n", log_iterator++);
	/* Update org base costs from XML */
	total_number_of_roles = count_roles(org_spec);
	/* Clear vector before adding new role elements */
	roles.clear();
	// This updates cost for ALL roles!
	for (uint8_t i = 0; i < total_number_of_roles; i++){
		Role r(i, org_spec, agent_id);
        // inserting objects to roles vector
        roles.push_back(r);
	}
	org_spec_changed = false;
	/* A change in the org-spec also requires to update the rewards for each role */
	rewards_changed = true;
	return;
}

/*************************************************************************************************/
/*!
 *  \brief  Gets called if the BDI-reasoner requests the new role rewards in the organization.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void on_bdi_get_rewards()
{
	LOG_INF("%i: BDI Get Rewards CB\n", log_iterator++);
	multiple_responses = roles.size();
	/* Get reward for each role */
	for (uint8_t i = 0; i < roles.size(); i++){
		/* Reward is different for the agent's current role */
		if (i == current_role && role_assigned == true) {
			coap_client_get_reward(roles[current_role].getRolePlayerUri().c_str());
		} else {
			coap_client_get_reward(roles[i].getRoleUri().c_str());
		}
		k_msleep(500);
	}
	return;
}

/*************************************************************************************************/
/*!
 *  \brief  Gets called if the BDI-reasoner requests the new local capabilities.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void on_bdi_get_local_costs() //TODO: Rename to more accurate name
{
	LOG_INF("%i: BDI Get local costs CB\n", log_iterator++);
	/* Now all costs/rewards are evaluated and we can find the best role */
	for (auto it = roles.begin(); it != roles.end(); ++it) {
		it->reevaluateRoleCosts(low_battery, 0);
	}
	best_role = find_best_role();
	LOG_INF("%i: Best role: %i; Reward: %i\n   Current role: %i; Reward: %i\n", log_iterator++, 
									best_role, roles[best_role].getTotalCostReward(), current_role, 
									roles[current_role].getTotalCostReward());
	if (current_role != best_role){
		optimal_state = false;
	}
	return;
}


/**************************************************************************************************
  OpenThread Callback-Functions.
**************************************************************************************************/

/**************************************************************************************************
  Sets BLUE LED on when connected.
**************************************************************************************************/
static void on_ot_connect(struct k_work *item)
{
	ARG_UNUSED(item);

	dk_set_led_on(BLUE_LED);
}

/**************************************************************************************************
  Sets BLUE LED off when disconnected.
**************************************************************************************************/
static void on_ot_disconnect(struct k_work *item)
{
	ARG_UNUSED(item);

	dk_set_led_off(BLUE_LED);
}

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
  CoAP Callback-Functions.
**************************************************************************************************/

/**************************************************************************************************
  Callback when organisational specification XML response arrived.
**************************************************************************************************/
static void on_coap_get_org_spec_xml(uint8_t *data, uint16_t size, 
									 uint8_t xml_part, uint8_t total_xml_parts)
{
	LOG_INF("%i: Received XML part %i of total %i parts\n", log_iterator++, xml_part+1, total_xml_parts);
	/* Add received XML part to local buffer */
	memcpy(org_spec+current_org_spec_lenght, data, size);
	std::string query = "segment=" + std::to_string(xml_part+1);
	/* Add received length to total org-spec XML length */
	current_org_spec_lenght = current_org_spec_lenght+size;
	
	if (xml_part < total_xml_parts-1){
		/* Not all XML parts received yet -->  Get next part of XML */
		LOG_INF("%i: Get next segment...\n", log_iterator++);
		coap_client_get_xml_segments("org-spec-segmented", query.c_str());
		return;
	} else {
		/* All XML parts received */
		org_spec_lenght = current_org_spec_lenght;
		LOG_INF("%i: Org-Spec total length: %i\n", log_iterator++, org_spec_lenght);
		current_org_spec_lenght = 0;
		org_spec[org_spec_lenght] = '\0';

		/* Indicate change in organization */
		org_spec_changed = true;

		if (start_agent == false) {
			start_agent = true;
			LOG_INF("%i: Wakeup Agent Thread\n", log_iterator++);
			k_wakeup(agent_run_thread_id);
		}
		return;
	}
}

/**************************************************************************************************
  Callback when CoAP GET role reward response arrives.
**************************************************************************************************/
static void on_coap_get_reward(int reward, const char *role_id, bool role_player)
{
	LOG_INF("%i: Received reward %i for: %s\n", log_iterator++, reward, role_id);
	if (0 < multiple_responses){ --multiple_responses; }

	// Add error handler when role not found --> update org-spec xml
	for (auto it = roles.begin(); it != roles.end(); ++it) {
    	std::string str(role_id);

		if (it->getRoleId() == str){
			if (it->getRoleNumber() == current_role && role_player == true && role_assigned == true){
				it->setOrgSpecReward(reward);
				if (reward >= 20) {it->setWorkloadDivider(2);}
				if (multiple_responses == 0){ local_capabilities_changed = true; }
			}
			if (it->getRoleNumber() == current_role && role_player == false && role_assigned == false){
				if (reward >= 20) {it->setWorkloadDivider(2);}
				it->setOrgSpecReward(reward);
				if (multiple_responses == 0){ local_capabilities_changed = true; }
			}
			if (it->getRoleNumber() != current_role && role_player == false){
				if (reward >= 20) {it->setWorkloadDivider(2);}
				it->setOrgSpecReward(reward);
				if (multiple_responses == 0){ local_capabilities_changed = true; }
			}
		}
	}
	return;
}
//bool end_flag = false;
/**************************************************************************************************
  Callback when POST role response arrives.
**************************************************************************************************/
static void on_coap_post_role(bool created)
{
	if (created){
		LOG_INF("%i: CoAP Role successfully posted on the server!\n", log_iterator++);
		current_role = find_best_role();
		optimal_state = true;
		role_assigned = true;
		role_deleted = false;
		ready_to_leave = true;
		local_capabilities_changed = false;
		/* Set size of measurement buffer */
		m_p_bme_sensor->resizeBuffer(roles[current_role].getBufferSize());
		/* make GET request on new roleplayer resource to observe */
		//coap_client_get_reward(roles[current_role].getRolePlayerUri().c_str());
		rewards_changed = true;
		/* Wakeup sensing thread */
		//if(end_flag) {end_of_experiment();}
		//end_flag = true;
		LOG_INF("%i: Wakeup Sensing Thread\n", log_iterator++);
		k_wakeup(sensing_thread_id);
	} else {
		LOG_WRN("%s","Warning: Role could not be posted on the server!\n");
	}
}

/**************************************************************************************************
  Callback when DELETE role response arrives.
**************************************************************************************************/
static void on_coap_delete_role(bool success)
{
	if(success){
		LOG_INF("%i: Successfully deleted: %i\n", log_iterator++, success);
		role_deleted = true;
	}
	return;
}

/**************************************************************************************************
  Button Actions.
**************************************************************************************************/
static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
	uint32_t buttons = button_state & has_changed;
	if (buttons & DK_BTN1_MSK) {
		dk_set_led_off(BLUE_LED);
		dk_set_led_off(RED_LED);
		dk_set_led_off(GREEN_LED);
		low_battery = !low_battery;
		if (!low_battery) {
			dk_set_led_on(GREEN_LED);
		} else {
			dk_set_led_on(RED_LED);
		}
	}else if (buttons & DK_BTN2_MSK) {
		// Nothing
	}
}


/**************************************************************************************************
  Local Functions.
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Evaluates the cost-reward function
 *
 *  \param  None.
 * 
 *  \return Role number of the current best role in terms of cost and reward.
 */
/*************************************************************************************************/
uint8_t find_best_role(void)
{
	LOG_INF("%i: Func find best role", log_iterator++);
	// TODO: Add iterator to take more than 2 roles into account
	if (roles[0].getTotalCostReward() >= roles[1].getTotalCostReward()){
		return 0;
	} else {
		return 1;
	}
}

/*************************************************************************************************/
/*!
 *  \brief  Evaluates the role costs.
 *
 *  \param  measurement_period: Operational period of the sensor.
 *  \param  update_period: Period of transmissions to the server.
 *  \param  battery_volt: Voltage of the battery in  mV.
 * 
 *  \return organizational costs.
 */
/*************************************************************************************************/


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
		if (role_assigned == false){
			k_sleep(K_FOREVER);
		}
		//LOG_INF("%i: Sensing Thread...\n", log_iterator++);
		/* Read BME688 enviromental sensor */		

		m_p_bme_sensor->measure();

		/* Release transmission when buffer is full */
		if (m_p_bme_sensor->isBufferFull())
		{ 
			k_sem_give(&sensor_sem);
		}

		if (roles[current_role].getWorkloadDivider() > 1){
			LOG_INF("%s","Sharing Workload!");
		}
		/* sleep */
		k_sleep(roles[current_role].getMeasurementPeriod());
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
		std::string str =  m_p_bme_sensor->getPayload() + ",battery_voltage:" + 
							std::to_string(battery_sample()) + ",current_reward:" + 
							std::to_string(roles[current_role].getTotalCostReward());
		coap_client_put_bme688("measurements/data", str.c_str());
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

	AgentSettings agent_settings;

	BeliefBase * beliefs = agent_settings.get_belief_base();
	EventBase * events = agent_settings.get_event_base();
	PlanBase * plans = agent_settings.get_plan_base();
	IntentionBase * intentions = agent_settings.get_intention_base();

	Agent agent(beliefs, events, plans, intentions);

	/* Run agent loop */
	while(true)
	{
		LOG_INF("%i: Agent...\n", log_iterator++);

		//gpio_pin_set_dt(&out2, 1); //GPIO
		for (int i = 0; i < REASONING_CYCLES; i++){
			agent.run();
		}
		//gpio_pin_set_dt(&out2, 0); //GPIO

		if (update_battery_level(&battery_level)){
			//start_of_experiment();
			LOG_INF("%i: Battery level changed to: %i\n", log_iterator++, battery_level);
			if (battery_level == 1){
				// Stop workload sharing when battery is high again
				roles[current_role].setWorkloadDivider(1);
			}
			local_capabilities_changed = true;
		}
		
		LOG_INF("%i: End of Agent...\n", log_iterator++);

		if (k_sem_count_get(&sensor_sem) >= 1){
			k_wakeup(transmission_thread_id);
		}
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
	CoapChannel coapChannel;
	//channelResponse resp = coapChannel.initialize();
	//LOG_INF("CoAP channel initialized. Response=%d", resp);

	//startThread();
	startThread(on_mtd_mode_toggle);
	k_msleep(2000);
	otLinkModeConfig mode = getCurrentMode();
	mode.mRxOnWhenIdle = 1;
	setCurrentMode(mode);
	k_msleep(2000);
	CoapClient::initialize();
	LOG_INF("Creating organization entity object %d", 2);
	Organization::refresh();
	
	/* Initialize battery measurement */
	// ret = battery_measure_enable(true);
	// if (ret) {
	// 	LOG_ERR("Failed initialize battery measurement (error: %d)", ret);
	// 	return 0;
	// }


	// /* Initialize BDI agent */
	// ret = bdi_agent_init(on_bdi_leave_role,
	// 					 on_bdi_enter_new_role,
	// 					 on_bdi_get_org_costs,
	// 					 on_bdi_get_rewards,
	// 					 on_bdi_get_local_costs);

	// if (ret) {
	// 	LOG_ERR("Failed to initialize BDI agent (error: %d)", ret);
	// 	return 0;
	// }

	// /* Initialize CoAP functionality */
	// ret = coap_client_utils_init(on_ot_connect, 
	// 							 on_ot_disconnect,
	// 							 on_mtd_mode_toggle,
	// 							 on_coap_get_org_spec_xml,
	// 							 on_coap_post_role,
	// 							 on_coap_delete_role,
	// 							 on_coap_get_reward);

	// if (ret) {
	// 	LOG_ERR("Failed to initialize CoAP utils (error: %d)", ret);
	// 	return 0;
	// }
	// k_msleep(2000);
	// /* Get organizational specification from server */
	// coap_client_get_xml_segments("org-spec-segmented", "segment=0");
}