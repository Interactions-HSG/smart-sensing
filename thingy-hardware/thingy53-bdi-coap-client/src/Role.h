#ifndef ROLE2_H
#define ROLE2_H

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

/**************************************************************************************************
  ZEPHYR include files.
**************************************************************************************************/
#include <zephyr/kernel.h>



/**************************************************************************************************
  Task include files.
**************************************************************************************************/
#include "agent/agent.h"
#include "config/configuration.h"
#include "bdi-agent/functions.h"
#include "parse_xml.h"
#include <coap_server_client_interface.h>

extern "C" {
  #include "coap_client_utils.h"
  //#include "sensors.h"
  //#include "battery.h"
  //#include "gpio.h"
}

/**************************************************************************************************
  Role Class.
**************************************************************************************************/
class Role {
	private:
		std::string m_group_id;
		std::string m_role_id;
		int m_group_number;
		int m_role_number;
		int m_measurement_period;
		int m_update_period;
		int m_role_costs;
		int m_org_spec_reward;
		int m_min_agents;
		int m_workload_divider;
        int m_battery_level = 1;
        std::string m_agent_id;

	public:
		Role(int role_number, const char *org_spec, std::string agent_id);
		void setOrgSpecReward(int reward);
		void reevaluateRoleCosts(bool override, int battery);
		void setWorkloadDivider(int number_of_agents);
		std::string getRoleId();
		int getRoleNumber();
		std::string getRoleUri();
		std::string getRolePlayerUri();
		int getTotalCostReward();
		int getBufferSize();
		int getWorkloadDivider();
		k_timeout_t getMeasurementPeriod();
};
#endif