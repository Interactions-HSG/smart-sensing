#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stdio.h>
#include <unistd.h>

#define DELAY 1000


/**************************************************************************************************
  Function Declarations
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
void setup();


/*************************************************************************************************/
/*!
 *  \brief  Action functions corresponding to agentspeak body instructions.
 *
 *  \param  None.
 *
 *  \return bool.
 */
/*************************************************************************************************/
bool action_turn_on_lights();

bool action_turn_off_lights();

bool action_raise_blinds();

bool action_lower_blinds();


/*************************************************************************************************/
/*!
 *  \brief  Update functions corresponding to agentspeak belief updates.
 *
 *  \param  bool var.
 *
 *  \return bool.
 */
/*************************************************************************************************/
bool update_user_turn_on(bool var);

bool update_user_turn_off(bool var);

bool update_sunny_outside(bool var);

bool update_cloudy_outside(bool var);

bool update_night_outside(bool var);

bool update_bright_inside(bool var);

bool update_dark_inside(bool var);


#endif /* FUNCTIONS_H_ */