/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : WriteUserSFlash.c
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

#include <Project.h>
#include <WriteUserSFlash.h>

/*******************************************************************************
* Function Name: WriteUserSFlashRow
********************************************************************************
* Summary:
*   This routine calls the PSoC 4 BLE device supervisory ROM APIs to update 
*   the user configuration area of Supervisory Flash (SFlash).  
*
* Parameters:
*   userRowNUmber - User config SFlash row number to which data is to be written
*   dataPointer - Pointer to the data to be written. This API writes one row of 
*                 user config SFlash row at a time.
*
* Return:
*   uint32_t - state of the user config SFlash write operation.
*******************************************************************************/
#if defined (__GNUC__)
#pragma GCC optimize ("O0")
#endif /* End of #if defined (__GNUC__) */
uint32_t WriteUserSFlashRow(uint8_t userRowNUmber, uint32_t *dataPointer)
{
    uint8_t localCount;
    volatile uint32_t retValue=0;
    volatile uint32_t cmdDataBuffer[(CY_FLASH_SIZEOF_ROW/4) + 2];
    volatile uint32_t reg1,reg2,reg3,reg4,reg5,reg6;
    
    /* Store the clock settings temporarily */
    reg1 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT));
    reg2 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG));
    reg3 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4));
    reg4 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5));
    reg5 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1));
    reg6 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2));
    
    /* Initialize the clock necessary for flash programming */
    CY_SET_REG32(CYREG_CPUSS_SYSARG, 0x0000e8b6);
    CY_SET_REG32(CYREG_CPUSS_SYSREQ, 0x80000015);
    
    /******* Initialize SRAM parameters for the LOAD FLASH command ******/
    /* byte 3 (i.e. 00) is the Macro_select */
    /* byte 2 (i.e. 00) is the Start addr of page latch */
    /* byte 1 (i.e. d7) is the key 2  */
    /* byte 0 (i.e. b6) is the key 1  */
    cmdDataBuffer[0]=0x0000d7b6;
    
    /****** Initialize SRAM parameters for the LOAD FLASH command ******/
    /* byte 3,2 and 1 are null */
    /* byte 0 (i.e. 7F) is the number of bytes to be written */
    cmdDataBuffer[1]=0x0000007F;     
    
    /* Initialize the SRAM buffer with data bytes */
    for(localCount = 0; localCount < (CY_FLASH_SIZEOF_ROW/4); localCount++)    
    {
        cmdDataBuffer[localCount + 2] = dataPointer[localCount]; 
    }
    
    /* Write the following to registers to execute a LOAD FLASH bytes */
    CY_SET_REG32(CYREG_CPUSS_SYSARG, &cmdDataBuffer[0]);
    CY_SET_REG32(CYREG_CPUSS_SYSREQ, LOAD_FLASH);
    
    /****** Initialize SRAM parameters for the WRITE ROW command ******/
    /* byte 3 & 2 are null */
    /* byte 1 (i.e. 0xeb) is the key 2  */
    /* byte 0 (i.e. 0xb6) is the key 1  */
    cmdDataBuffer[0] = 0x0000ebb6;
    
    /* byte 7,6 and 5 are null */
    /* byte 4 is desired SFlash user row 
     * Allowed values 0 - row 4
                      1 - row 5 
                      2 - row 6
                      3 - row 7 */
    cmdDataBuffer[1] = (uint32_t) userRowNUmber;
    
    /* Write the following to registers to execute a WRITE USER SFlash ROW command */
    CY_SET_REG32(CYREG_CPUSS_SYSARG, &cmdDataBuffer[0]);
    CY_SET_REG32(CYREG_CPUSS_SYSREQ, WRITE_USER_SFLASH_ROW);
    
    /* Read back SYSARG for the result. 0xA0000000 = SUCCESS; */
    retValue = CY_GET_REG32(CYREG_CPUSS_SYSARG);
    
    /* Restore the clock settings after the flash programming is done */
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT),reg1);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG),reg2);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4),reg3);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5),reg4);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1),reg5);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2),reg6);  
    
    return retValue;
}
#if defined (__GNUC__)
#pragma GCC reset_options
#endif /* End of #if defined (__GNUC__) */

/* [] END OF FILE */
