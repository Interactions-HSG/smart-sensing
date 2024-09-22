/******************************************************************************
* Project Name      : Simple BLE
* File Name         : main.c
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

#include <main.h>
#include <stdbool.h>

/******************************************************************************
* Global Variable Declarations
******************************************************************************/
uint8 restartadvertisement = false;             /* restart advertisement flag               */
int32 interval = (1500u);                       /* advertisement interval ( millisecond )   */

uint8 wdt_trigger_on_flag = false;              /* WDT_INTERRUPT_SOURCE trigger flag        */
BEACON_APP_STATE        beacon_state = BEACON_WAIT;  /* Beacon state flag                   */

/******************************************************************************
* Function prototypes
******************************************************************************/
CYBLE_API_RESULT_T Beacon_GappStartAdvertisement(uint32 advertisingInterval);

void ProcessBeaconEvents(void);

/******************************************************************************
* Function Name: BLE_AppEventHandler
*******************************************************************************
*
* Summary:
*   BLE stack generic event handler routine for handling discovery event.
*
* Parameters:  
*   event - event that triggered this callback
*   eventParam - parameters for the event.
*
* Return: 
*   None
******************************************************************************/
void BLE_AppEventHandler(uint32 event, void* eventParam)
{
    (void)eventParam;

    switch (event)
    {
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CYBLE_EVT_STACK_ON:
            /* This event is received when component is Started. */
            restartadvertisement = true;
            
            CySysWdtUnlock(); /* Unlock the WDT registers for modification */

            CySysWdtWriteMode(SOURCE_COUNTER, CY_SYS_WDT_MODE_INT);
            
            CySysWdtWriteClearOnMatch(SOURCE_COUNTER, COUNTER_ENABLE);
            
            CySysWdtWriteMatch(SOURCE_COUNTER, COUNT_PERIOD_1S);
            
            CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
            
            CySysWdtLock();
        break;
            
        default:
        break;
    }
}

/******************************************************************************
* Function Name: WDT_Handler
*******************************************************************************
*
* Summary:
*   WDT interrupt handler routine. WDT is used to wakeup the device after
*   the expected time elapses.
*
* Parameters:  
*   None
*
* Return: 
*   None
******************************************************************************/
CY_ISR(WDT_Handler)
{
    /* If SOURCE_COUNTER triggers the interrupt. */
    if(CySysWdtGetInterruptSource() & WDT_INTERRUPT_SOURCE)
    {
        CySysWdtClearInterrupt(WDT_INTERRUPT_SOURCE);
    }
    
    /* If I2C_COUNTER triggers the interrupt. */
    if(CySysWdtGetInterruptSource() & I2C_WDT_INTERRUPT_SOURCE)
    {
        CySysWdtClearInterrupt(I2C_WDT_INTERRUPT_SOURCE);
        
        wdt_trigger_on_flag = true;
    }
}

/*******************************************************************************
* Function Name: LowPower_WCO_ECO_Start
********************************************************************************
* Summary:
*   Start WCO & ECO in low power mode by configuring the system in DeepSleep mode during
*   WCO & ECO startup time.
*
* Parameters:
*   None
*
* Return:
*   None
*******************************************************************************/
void LowPower_WCO_ECO_Start(void)
{   
    /* Do the following:
     * 1. Shut down the ECO (to reduce power consumption while WCO is starting)
     * 2. Enable WDT to wakeup the system after 2s (WCO need 500ms to startup, 
     *    the solar cell can gather more power during the 2 seconds.)
     * 3. Configure PRoC BLE device in DeepSleep mode for the 2s WCO startup time
     * 4. After WCO is enabled, restart the ECO so that BLESS interface can function
     * 5. Enable WDT to wakeup the system after 3.5ms (3.5ms = ECO startup time)
     * 6. Configure PRoC BLE device in DeepSleep mode for the 3.5ms ECO startup time 
     */
    CySysClkEcoStop();      /* Shutdown the ECO and later re-start in low power mode after WCO is turned on */
    
    WDT_Interrupt_StartEx(WDT_Handler);     /* Initialize WDT interrupt */
    
    CySysClkWcoStart();     /* Start the WCO clock */
    
    CySysWdtUnlock();       /* Unlock the WDT registers for modification */
    
    CySysWdtWriteMode(SOURCE_COUNTER, CY_SYS_WDT_MODE_INT);
    CySysWdtWriteClearOnMatch(SOURCE_COUNTER, COUNTER_ENABLE);
    CySysWdtWriteMatch(SOURCE_COUNTER, COUNT_PERIOD_WCO);
    
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    
    CySysWdtLock();
    
    CySysPmDeepSleep(); /* Wait for the WDT interrupt to wake up the device */
    
    (void)CySysClkWcoSetPowerMode(CY_SYS_CLK_WCO_LPM);  /* Switch WCO to the low power mode after startup */
    
    /* Unlock to change the clock source for the LFCLK clock. */
    CySysWdtUnlock();
    CySysClkSetLfclkSource(CY_SYS_CLK_LFCLK_SRC_WCO);   /* LFCLK is now driven by WCO */
    CySysWdtLock();
    
    CySysClkIloStop();                                  /* WCO is running, shut down the ILO */
    
    (void)CySysClkEcoStart(0);                          /* It's time to start ECO */
    
    CySysWdtUnlock();
    
    CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
    
    CySysWdtWriteMatch(SOURCE_COUNTER, COUNT_PERIOD_ECO);
    
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    
    CySysWdtLock();
    
    CySysPmDeepSleep(); /* Wait for the WDT interrupt to wake up the device */
    
    /* Disable WDT counter */
    CySysWdtUnlock();
    CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
    CySysWdtLock();
}

/******************************************************************************
* Function Name: main
*******************************************************************************
* Summary:
*   Solar BLE Sensor entry point. This calls the BLE and other peripheral Component
*   APIs for achieving the desired system behaviour
*
* Parameters:
*   None
*
* Return:
*   int - this is main loop and never returns
*
******************************************************************************/
int main()
{
    CyGlobalIntEnable;

    /* Set the divider for ECO, ECO will be used as source when IMO is switched off to save power, to drive the HFCLK */
    CySysClkWriteEcoDiv(CY_SYS_CLK_ECO_DIV8);

    /* Start WCO & ECO in low power mode */
    LowPower_WCO_ECO_Start();

    CyBle_Start(BLE_AppEventHandler);
    CyBle_ProcessEvents();      /* ReadAndApplyConfig will be invoked to set TxPower, Sensor_Flag, etc. */
    
    for(;;)
    {
        CYBLE_LP_MODE_T pwrState;
        CYBLE_BLESS_STATE_T blessState;
        uint8 intStatus = 0;
        
        CyBle_ProcessEvents(); /* BLE stack processing state machine interface */
        
        /**********************************************************************************************/
        pwrState  = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP); /* Configure BLESS in Deep-Sleep mode */

        intStatus = CyEnterCriticalSection(); /* No interrupts allowed while entering system low power modes */
        
        blessState = CyBle_GetBleSsState();

        /* Make sure BLESS is in Deep-Sleep before configuring system in Deep-Sleep mode */
        if(pwrState == CYBLE_BLESS_DEEPSLEEP)
        {
            /* If BLESS is in Deep Sleep or is in the process of waking up from Deep Sleep,
             * put system in Deep Sleep mode */
            if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
            {
                CySysPmDeepSleep(); /* System Deep-Sleep. 1.3uA mode */
            }
        }
        /* If BLESS is in Active state, 
         * and if BLESS Tx/Rx Event is not complete, stop IMO and put CPU to Sleep */
        else if (blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
            /* Change HF clock source from IMO to ECO, as IMO can be stopped to save power */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO); 
            
            /* Stop IMO for reducing power consumption */
            CySysClkImoStop(); 
          
            /* Put the CPU to Sleep. 1.1mA mode */
            CySysPmSleep();
           
            /* Starts execution after waking up, start IMO */
            CySysClkImoStart();
            
            /* Change HF clock source back to IMO */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
        }
        
        CyExitCriticalSection(intStatus);
        /**********************************************************************************************/
        
        /* If restartadvertisement flag is set, which means it's the first time cpu goes here. */
        if(restartadvertisement)
        {
            restartadvertisement = false;
            
            CySysWdtUnlock();
            
            CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
            
            CySysWdtWriteMode(I2C_COUNTER, CY_SYS_WDT_MODE_INT);
            
            CySysWdtWriteClearOnMatch(I2C_COUNTER, COUNTER_ENABLE);
            
            CySysWdtWriteMatch(I2C_COUNTER, I2C_COUNT_PERIOD_10MS);
                
            CySysWdtEnable(CY_SYS_WDT_COUNTER1_MASK);
                
            CySysWdtLock();            
        }

        ProcessBeaconEvents();
    }
}

/******************************************************************************
* Function Name: ProcessBeaconEvents
*******************************************************************************
*
* Summary:
*   This function handles Beacon events according to current state and update the 
*   state value.
*
* Parameters:
*   None
*
* Return:
*   None
*
******************************************************************************/
void ProcessBeaconEvents(void)
{
    /* If I2C_COUNTER triggers a new interrupt after the expected time elapsed. */
    if(wdt_trigger_on_flag)
    {
        CYBLE_API_RESULT_T apiResult;
        
        switch(beacon_state)
        {
            case BEACON_WAIT:
                /* Wait 1 second to earn enough power to start advertising */
                beacon_state = BEACON_START;
            break;
                
            case BEACON_START:
                /* Stop I2C_COUNTER, and start advertisement */
                CySysWdtUnlock(); /* Unlock the WDT registers for modification */
                
                CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);
                
                CySysWdtLock();
                
                apiResult = Beacon_GappStartAdvertisement(interval);
                /* If fails to start advertisement, halt the processor. */
                if(apiResult != CYBLE_ERROR_OK)
                {
                    CYASSERT(0);
                }
                beacon_state = BEACON_RUN;
            break;
                
            default:
            break;
        }
        wdt_trigger_on_flag = false;
    }
}

/******************************************************************************
* Function Name: Beacon_GappStartAdvertisement
*******************************************************************************
*
* Summary:
*   This function is used to start the advertisement using the given advertising 
*   interval and advertisement data stored in cyBle_discoveryData.advData
*
* Parameters:
*   advertisingInterval - Time in millisecond for advertising interval 
*
* Return:
*   CYBLE_API_RESULT_T - Return value indicates if the function succeeded or 
*   failed. Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   CYBLE_ERROR_OK                      On successful operation.
*   CYBLE_ERROR_INVALID_PARAMETER       On passing an invalid parameter.
******************************************************************************/
CYBLE_API_RESULT_T Beacon_GappStartAdvertisement(uint32 advertisingInterval)
{
    uint16 RegisterIntervalValue;
    
    RegisterIntervalValue = CALC_REG_INTERVAL(advertisingInterval);
    /* Set advertising broadcast interval*/
    cyBle_discoveryModeInfo.advParam->advIntvMin = RegisterIntervalValue;
    cyBle_discoveryModeInfo.advParam->advIntvMax = RegisterIntervalValue;

    /* Start advertising */
    return  CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_CUSTOM);
}

/* [] END OF FILE */
