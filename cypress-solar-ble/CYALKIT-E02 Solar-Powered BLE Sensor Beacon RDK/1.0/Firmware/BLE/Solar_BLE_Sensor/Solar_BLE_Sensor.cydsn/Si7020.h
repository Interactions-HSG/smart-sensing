/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : Si7020.h
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

#include <project.h>

#ifndef __SI7020_H_
#define __SI7020_H_

/******************************************************************************
* Macros and Constants
******************************************************************************/
#define SI7020_SLAVE_ADDR               (0x40)    /* Sensor's I2C slave address */

#define SI7020_WRITE_USER_REG           (0xE6)    /* I2C command for Writing RH/T User Register */
#define SI7020_READ_USER_REG            (0xE7)    /* I2C command for Reading RH/T User Register */
#define SI7020_MEASURE_RH               (0xF5)    /* I2C command for Measuring Relative Humidity */
#define SI7020_READ_TEMP                (0xE0)    /* I2C command for Reading Temperature Value from Previous RH Measurement */

#define SI7020_MEASURE_RH_SEND_LEN      (0x01)    /* I2C command send length for Measuring Relative Humidity */
#define SI7020_MEASURE_RH_RECV_LEN      (0x02)    /* I2C command receive length for Measuring Relative Humidity */
#define SI7020_READ_TEMP_SEND_LEN       (0x01)    /* I2C command send length for Reading Temperature Value from Previous RH Measurement */
#define SI7020_READ_TEMP_RECV_LEN       (0x02)    /* I2C command receive length for Reading Temperature Value from Previous RH Measurement */

#define USER_REG_SETTING                (0x3A | 0x01) /* Measurement Resolution: RH 8bit TEMP:12bit */

#define I2C_BUFFER_SIZE                 (4u)        /* I2C buffer size */
#define I2C_BUFFER_TEM_OFFSET_CMD       2           /* Position offset of temperature command in I2C_buffer */
#define I2C_BUFFER_TEM_OFFSET_DATA      2           /* Position offset of temperature data in I2C_buffer */
#define I2C_BUFFER_HUM_OFFSET_CMD       0           /* Position offset of humidity command in I2C_buffer */
#define I2C_BUFFER_HUM_OFFSET_DATA      0           /* Position offset of humidity data in I2C_buffer */

/* Measuring Temperature and Relative Humidity */
#define SIZEOF_I2C_SEND_CMD             1           /* Size of 1 command to I2C R/W function */
#define SIZEOF_I2C_RCV_DATA             2           /* Size of received data from I2C R/W function */
/* Reading and Writing User Registers */
#define SI7020_WRITE_REG_SEND_LEN       (0x02)      /* I2C command send length for User Register */
#define SI7020_WRITE_REG_INDEX_CMD      0           /* Send the command after slave address when Writing User Register */
#define SI7020_WRITE_REG_INDEX_DATA     1           /* Send the data to be written after the command when Writing User Register */
    
/* The standard formula to calculate the temperature:
 *      temperature = temp_16bit*175.72/65536 - 46.85
 * 2^16 = 65536. So, we have the following formula:
 *      temperature*100 = (temp_16bit*17572)/65536 - 4685
 *                      = (temp_16bit*17572) >> 16 - 4685
 * We use 0.01Celsius as the Unit.
 *      temperature = (temp_16bit*17572)>>16 - 4685;
 */
#define CALC_TEMP_100(REG_V_16bit)      ((int16_t)(((REG_V_16bit) * 17572) >> 16) - (int16_t)4685)
    
#endif /* __SI7020_H_ */

/* [] END OF FILE */
