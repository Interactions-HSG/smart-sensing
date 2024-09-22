/******************************************************************************
* Project Name      : Simple BLE
* File Name         : main.h
* Version           : CYALKIT-E02 Sample Firmware, Version 1.2.00
* Device Used       : CYBLE-022001-00
* Software Used     : PSoC Creator 3.3 CP3
* Compiler          : ARM GCC 4.9.3
* Related Hardware  : Solar BLE Sensor
*
*******************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation. All Rights Reserved.
*******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
******************************************************************************/

#if !defined (MAIN_H)
#define MAIN_H
    
#include <project.h>

/******************************************************************************
* Macros and Constants
******************************************************************************/
/* WDT counter settings */
#define COUNTER_ENABLE                          (1u)        /* 1 to enable WDT counter */
#define WCO_FREQUENCY                           32768       /* 32768Hz */
/* WDT counter for low power sleep mode */
#define SOURCE_COUNTER                          (0u)        /* use WDT counter 0 */
#define COUNT_PERIOD_WCO                        ((uint32)(WCO_FREQUENCY/2 - 1))     /* 500ms */
#define COUNT_PERIOD_ECO                        ((uint32)115)               /* 3.5ms */
#define COUNT_PERIOD_1S                         ((uint32)(WCO_FREQUENCY - 1))       /* 1s */
#define WDT_INTERRUPT_SOURCE                    CY_SYS_WDT_COUNTER0_INT /* WDT counter 0 interrupt pending bit */
/* WDT counter for low power sleep mode and setting flag */
#define I2C_COUNTER                             (1u)        /* use WDT counter 1 */
#define I2C_COUNT_PERIOD_10MS                   ((uint32)(WCO_FREQUENCY*10/1000 - 1))   /* 10ms */
#define I2C_WDT_INTERRUPT_SOURCE                CY_SYS_WDT_COUNTER1_INT /* WDT counter 1 interrupt pending bit */

/*
 * Value assigned to advIntvMin or advIntvMax = (Time in millisecond)/0.625
 *                                            = (Time in millisecond)*8/5
 *                                            = ((Time in millisecond) << 3) / 5
 */
#define CALC_REG_INTERVAL(T_ms)                 ((T_ms << 3) / 5)

/******************************************************************************
* Enumerated Data Definition
******************************************************************************/
/* Beacon state machine.
 * To lower power requirement, advertisement won't start immediately after the 
 * system is initialized when sensor setting is off. */
typedef enum
{
    /* Default state. Wait after the system is initialized. 
     * The next state is BEACON_START. */
    BEACON_WAIT = 0x00,
    /* Start advertisement when the current state is BEACON_START. 
     * The next state is BEACON_RUN. */
    BEACON_START,
    /* The system is advertising. There's no next state. */
    BEACON_RUN
}BEACON_APP_STATE;

#endif  /* MAIN_H */

/* [] END OF FILE */
