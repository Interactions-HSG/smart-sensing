/*******************************************************************************
* File Name: USB_Detect.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "USB_Detect.h"

static USB_Detect_BACKUP_STRUCT  USB_Detect_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: USB_Detect_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet USB_Detect_SUT.c usage_USB_Detect_Sleep_Wakeup
*******************************************************************************/
void USB_Detect_Sleep(void)
{
    #if defined(USB_Detect__PC)
        USB_Detect_backup.pcState = USB_Detect_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            USB_Detect_backup.usbState = USB_Detect_CR1_REG;
            USB_Detect_USB_POWER_REG |= USB_Detect_USBIO_ENTER_SLEEP;
            USB_Detect_CR1_REG &= USB_Detect_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(USB_Detect__SIO)
        USB_Detect_backup.sioState = USB_Detect_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        USB_Detect_SIO_REG &= (uint32)(~USB_Detect_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: USB_Detect_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to USB_Detect_Sleep() for an example usage.
*******************************************************************************/
void USB_Detect_Wakeup(void)
{
    #if defined(USB_Detect__PC)
        USB_Detect_PC = USB_Detect_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            USB_Detect_USB_POWER_REG &= USB_Detect_USBIO_EXIT_SLEEP_PH1;
            USB_Detect_CR1_REG = USB_Detect_backup.usbState;
            USB_Detect_USB_POWER_REG &= USB_Detect_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(USB_Detect__SIO)
        USB_Detect_SIO_REG = USB_Detect_backup.sioState;
    #endif
}


/* [] END OF FILE */
