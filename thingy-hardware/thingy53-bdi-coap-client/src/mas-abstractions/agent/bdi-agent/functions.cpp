#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/settings/settings.h>
#include <dk_buttons_and_leds.h>
#include <algorithm>
#include <main.h>


/**************************************************************************************************
  Task include files.
**************************************************************************************************/
#include "functions.h"
#include <coap_server_client_interface.h>


extern "C" {
  #include "coap_client_utils.h"
 // #include "sensors.h"
}

LOG_MODULE_REGISTER(functions, CONFIG_COAP_CLIENT_LOG_LEVEL);

/**************************************************************************************************
  Callback functions for main.cpp
**************************************************************************************************/
leave_role_cb_t on_bdi_leave_role;
enter_new_role_cb_t on_bdi_enter_new_role;
get_org_spec_costs_cb_t on_bdi_get_org_spec_costs;
get_rewards_cb_t on_bdi_get_rewards;
get_local_costs_cb_t on_bdi_get_local_costs;


/**************************************************************************************************
  Functions.
**************************************************************************************************/
/*************************************************************************************************/
/*!
 *  \brief  Setup function.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
int bdi_agent_init(leave_role_cb_t on_leave,
				   enter_new_role_cb_t on_enter,
                   get_org_spec_costs_cb_t on_get_costs,
                   get_rewards_cb_t on_get_rewards,
                   get_local_costs_cb_t on_get_local_costs)
{
	on_bdi_leave_role = on_leave;
	on_bdi_enter_new_role = on_enter;
	on_bdi_get_org_spec_costs = on_get_costs;
	on_bdi_get_rewards = on_get_rewards;
	on_bdi_get_local_costs = on_get_local_costs;
	return 0;
}

/**************************************************************************************************
  Agentspeak Functions.
**************************************************************************************************/
/*************************************************************************************************/
/*!
 *  \brief  Action functions corresponding to agentspeak body instructions.
 *
 *  \param  None.
 *
 *  \return bool.
 */
/*************************************************************************************************/
bool action_get_org_spec_costs(){
	// Calculate cost-reward for all roles
	//LOG_INF("Action: Get Org Spec Costs\n");
	org_spec_changed = false;
	on_bdi_get_org_spec_costs();
	return true;
}

bool action_get_rewards(){
	// Calculate cost-rewards for all roles
	rewards_changed = false;
	//LOG_INF("Action: Get Rewards\n");
	on_bdi_get_rewards();
	return true;
}

bool action_get_local_costs(){
	// Calculate cost-reward for all roles
	//LOG_INF("Action: Get Local Costs\n");
	local_capabilities_changed = false;
	on_bdi_get_local_costs();
	return true;
}

bool action_leave_role(){
	//LOG_INF("Action: Leave Role\n");
	role_assigned = false;
	on_bdi_leave_role();
	return true;
}

bool action_enter_new_role(){
	//LOG_INF("Action: Enter new role\n");
	role_deleted = false;
	on_bdi_enter_new_role();
	return true;
}
/*************************************************************************************************/
/*!
 *  \brief  Update functions corresponding to agentspeak belief updates.
 *
 *  \param  bool var.
 *
 *  \return bool.
 */
/*************************************************************************************************/
bool update_agent_on(bool var){
	/* Starts agentspeak reasoning */
	if (start_agent == true){
		return true;
	}
	return false;
}

bool update_org_spec_changed(bool var){
	/* The agent got notified that the org-spec changed */
	if (org_spec_changed == true){
		return true;
	}
	return false;
}

bool update_rewards_changed(bool var){
	/* The agent got notified that the reward of his role changed*/
	if (rewards_changed == true){
		return true;
	}
	return false;
}

bool update_local_capabilities_changed(bool var){
	/* The agent got notified that the reward of his role changed*/
	if (local_capabilities_changed == true){
		return true;
	}
	return false;
}

bool update_not_optimal_state(bool var){
	/* Not optimal state if and only if there is another role AND this role can be entered */
	if (optimal_state == false){
		return true;
	}
	return false;
}

bool update_role_assigned(bool var){
	if (role_assigned == true){
		return true;
	}
	return false;
}

bool update_role_deleted(bool var){
	if (role_deleted == true){
		return true;
	}
	return false;
}