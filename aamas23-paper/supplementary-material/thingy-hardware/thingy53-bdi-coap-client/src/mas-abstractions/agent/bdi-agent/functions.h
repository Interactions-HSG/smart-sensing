#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_


/**************************************************************************************************
  Includes.
**************************************************************************************************/
#include <stdio.h>
#include <unistd.h>


/**************************************************************************************************
  Function Declarations.
**************************************************************************************************/
typedef void (*leave_role_cb_t)(void);

typedef void (*enter_new_role_cb_t)(void);

typedef void (*get_org_spec_costs_cb_t)(void);

typedef void (*get_rewards_cb_t)(void);

typedef void (*get_local_costs_cb_t)(void);

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
                   get_local_costs_cb_t on_get_local_costs);


/**************************************************************************************************
  Agentspeak Function Declarations
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
bool action_get_org_spec_costs();

bool action_get_rewards();

bool action_get_local_costs();

bool action_leave_role();

bool action_enter_new_role();

/*************************************************************************************************/
/*!
 *  \brief  Update functions corresponding to agentspeak belief updates.
 *
 *  \param  bool var.
 *
 *  \return bool.
 */
/*************************************************************************************************/
bool update_agent_on(bool var);

bool update_org_spec_changed(bool var);

bool update_rewards_changed(bool var);

bool update_local_capabilities_changed(bool var);

bool update_not_optimal_state(bool var);

bool update_role_deleted(bool var);

bool update_role_assigned(bool var);

#endif /* FUNCTIONS_H_ */
