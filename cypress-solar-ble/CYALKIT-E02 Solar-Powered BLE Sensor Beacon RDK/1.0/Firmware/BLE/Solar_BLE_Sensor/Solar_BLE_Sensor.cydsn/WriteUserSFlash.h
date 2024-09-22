/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : WriteUserSFlash.h
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
*******************************************************************************/

#ifndef WRITE_USER_SFLASH_H
#define WRITE_USER_SFLASH_H

#include <project.h>
    
/*******************************************************************************
* Enums and macros
*******************************************************************************/
#define USER_SFLASH_ROW_SIZE            (128u) /* SFlash row size for 128KB flash BLE device. For other PSoC 4 BLE devices 
                                                * with higher flash size, this example project might need some modification.
                                                * Please check the device datasheet and TRM before using this code on non 128KB
                                                * flash devices */

#define USER_SFLASH_ROWS                (4u)   /* Total number of user SFlash rows supported by the device */
    
#define USER_SFLASH_BASE_ADDRESS        (0x0FFFF200u) /* Starting address of user SFlash row for 128KB PSoC 4 BLE device */
    
#define LOAD_FLASH                      (0x80000004u) /* LOAD FLASH bytes command for CY_SET_REG32 */
#define WRITE_USER_SFLASH_ROW           (0x80000018u) /* WRITE USER SFlash ROW command for CY_SET_REG32 */
#define USER_SFLASH_WRITE_SUCCESSFUL    (0xA0000000u) /* WriteUserSFlashRow() return 0xA0000000 means write successfully. */

/*******************************************************************************
* Function declarations
*******************************************************************************/
uint32_t WriteUserSFlashRow(uint8_t userRowNUmber, uint32_t *dataPointer);   

#endif /* End of #ifndef WRITE_USER_SFLASH_H */

/* [] END OF FILE */
