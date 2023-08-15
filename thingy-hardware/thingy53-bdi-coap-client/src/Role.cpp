#include "Role.h"
/**************************************************************************************************
  Role Class.
**************************************************************************************************/
Role::Role(int role_number, const char *org_spec, std::string agent_id) {
	get_group_and_role_id(role_number, &m_group_id, &m_role_id, &m_group_number, org_spec);
	m_role_number = role_number;
	get_role_property(m_role_id, "update_period", &m_update_period, org_spec);
	get_role_property(m_role_id, "measurement_period", &m_measurement_period, org_spec);
	get_group_spec(m_role_id, "min", &m_min_agents, org_spec);
	m_workload_divider = 1;
	m_role_costs = 0;
	reevaluateRoleCosts(false,1);
    m_agent_id = agent_id;
}

void Role::setOrgSpecReward(int reward)
{
	m_org_spec_reward = reward;
	return;
}

void Role::reevaluateRoleCosts(bool override, int battery)
{    
    if(!override){
		//update_battery_level(&m_battery_level);
	}else{
		m_battery_level = battery;
	}
	/* Lower Frequency */
	if (30 < m_measurement_period) {
		m_role_costs = m_battery_level == 1 ? -1 : 0;
	}
	else if (m_measurement_period <= 30) {
		m_role_costs = m_battery_level == 1 ? 0 : 2;
	}
	return;
}

void Role::setWorkloadDivider(int number_of_agents)
{
	m_workload_divider = number_of_agents;
	return;
}

int Role::getTotalCostReward()
{
	return m_org_spec_reward-m_role_costs;
}

int Role::getRoleNumber()
{
	return m_role_number;
}

std::string Role::getRoleId()
{
	return m_role_id;
}

std::string Role::getRoleUri()
{
	std::string uri = "org-entities/oe-groups/gr_0" + std::to_string(m_group_number) + "_" +
	m_group_id + "/gr_0" + std::to_string(m_group_number) + "_" + m_group_id + "_" + m_role_id;
	return uri;
}

std::string Role::getRolePlayerUri()
{
	std::string uri = "org-entities/oe-groups/gr_0" + std::to_string(m_group_number) + "_" +
	m_group_id + "/gr_0" + std::to_string(m_group_number) + "_" + m_group_id + "_" + m_role_id;
	
	uri = uri + "/" + "rp_" + m_agent_id + "_gr_0" + std::to_string(m_group_number) + "_" +
	m_group_id + "_" + m_role_id;
	return uri;
}

int Role::getBufferSize(){
	return m_update_period/m_measurement_period;
}

k_timeout_t Role::getMeasurementPeriod(){
	return K_MSEC(m_workload_divider*m_measurement_period*1000);
}

int Role::getWorkloadDivider()
{
	return m_workload_divider;
}
