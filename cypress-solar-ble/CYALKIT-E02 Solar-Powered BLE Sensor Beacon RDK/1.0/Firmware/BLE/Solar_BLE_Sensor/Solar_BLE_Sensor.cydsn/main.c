/******************************************************************************
* Project Name      : Solar BLE Sensor
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
#include <Si7020.h>
#include "UartCmd.h"
#include "ConfigRW.h"
#include "Eddystone.h"


/******************************************************************************
* Global Variable Declarations
******************************************************************************/
uint8_t restartadvertisement = false;           /* restart advertisement flag               */
static BeaconConfig *pCFG;                      /* pointer to the valid configuration       */
static uint16_t CFG_interval;                   /* advertisement interval ( millisecond )   */
static int8_t CFG_txpwr;                        /* TX Power setting                         */

#if (I2C_SENSOR_ENABLE)
uint8_t I2C_buffer[I2C_BUFFER_SIZE];            /* buffer for I2C communication with Si7020 */
I2C_SENSOR_APP_STATE i2c_state = I2C_START;     /* I2C state flag                           */
int8_t Sensor_Flag;                             /* flag indicates that sensor is used or not*/
#endif
uint8_t wdt_trigger_on_flag = false;            /* WDT_INTERRUPT_SOURCE trigger flag        */
BEACON_APP_STATE beacon_state = BEACON_WAIT;    /* Beacon state flag                        */

/* A copy of Eddystone-UID frame. It's AD DATA 3 in the advertisement packet. */
static uint8_t UID_FrameBuffer[ES_UID_FRAME_ARRAY_LENGTH] = 
{
    ES_UID_FRAME_LENGTH, ES_SVC_DATA, ES_SVC_LSB, ES_SVC_MSB, ES_FRAME_TYPE_UID
};

/* A copy of Eddystone-URL frame. It's AD DATA 3 in the advertisement packet. */
static uint8_t URL_FrameBuffer[ES_URL_FRAME_ARRAY_MAXLEN] = 
{
    0x00, /* This byte stores the actual length after it. ReadAndApplyConfig() will init it. */
    ES_SVC_DATA, ES_SVC_LSB, ES_SVC_MSB, ES_FRAME_TYPE_URL
};

/* A copy of Eddystone-TLM frame. It's AD DATA 3 in the advertisement packet. */
static const uint8_t DefaultTLMframe[ES_TLM_FRAME_ARRAY_LENGTH] = 
{
    ES_TLM_FRAME_LENGTH, ES_SVC_DATA, ES_SVC_LSB, ES_SVC_MSB, ES_FRAME_TYPE_TLM, 
    0x00,                           /* TLM version */
    ES_TLM_VBATT_DEFAULT >> 8,      /* MSB */
    ES_TLM_VBATT_DEFAULT & 0xFF,    /* LSB */
    /* Beacon temperature in Celsius (8.8 fixed point notation) */
    ES_TLM_NOTEMP_MSB, 
    ES_TLM_NOTEMP_LSB
};

static uint32_t ES_ADV_CNT;             /* Advertising PDU count */
static uint32_t ES_SEC_CNT;             /* Unit: 0.1sec. Time since power-on or reboot. */
static uint32_t ES_SEC_CNT_ms;          /* Unit: ms. Actual time is ES_SEC_CNT + ES_SEC_CNT_ms/100. */


/******************************************************************************
* Function prototypes
******************************************************************************/
void LowPowerSleep(void);
void ReadAndApplyConfig(void);
void ProcessBeaconEvents(void);
void UpdateTxPowerLeverl(int8_t NewLevel);
void UpdateAdData3(uint8_t NewEdFrame);
CYBLE_API_RESULT_T Beacon_GappStartAdvertisement(uint32_t advertisingInterval);

#if I2C_SENSOR_ENABLE
void ProcessI2CEvents(void);
static void UpdateWdtCounter1(uint32_t match);
#endif


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
void BLE_AppEventHandler(uint32_t event, void* eventParam)
{
    (void)eventParam;

    switch (event)
    {
        /**********************************************************
        *                       General Events
        ***********************************************************/
        /* This event is received when component is Started. */
        case CYBLE_EVT_STACK_ON:
            
            /* UpdateTxPowerLeverl() should be invoked in the case of CYBLE_EVT_STACK_ON. */
            UpdateTxPowerLeverl(CFG_txpwr);
            
            restartadvertisement = true;
            
            CySysWdtUnlock(); /* Unlock the WDT registers for modification */
            
            CySysWdtWriteMode(SOURCE_COUNTER, CY_SYS_WDT_MODE_INT);
            
            CySysWdtWriteClearOnMatch(SOURCE_COUNTER, COUNTER_ENABLE);
            
            CySysWdtWriteMatch(SOURCE_COUNTER, COUNT_PERIOD_100MS);
            
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
*   Start WCO & ECO in low power mode by configuring the system in DeepSleep mode 
*   during WCO & ECO startup time.
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

/*******************************************************************************
* Function Name: LowPowerSleep
********************************************************************************
* Summary:
*   Put System into Deep-Sleep mode. Put BLESS into Deep-Sleep mode if possible.
*
* Parameters:
*   None
*
* Return:
*   None
*******************************************************************************/
void LowPowerSleep(void)
{
    CYBLE_LP_MODE_T pwrState;
    CYBLE_BLESS_STATE_T blessState;
    uint8_t intStatus = 0;

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
    
    return;
}

/******************************************************************************
* Function Name: main
*******************************************************************************
* Summary:
*   Program entry point. This calls the BLE and other peripheral Component
*   APIs for achieving the desired system behaviour.
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

    /* If USB connector is plugged in. */
    if(USB_Detect_Read() == PIN_HIGH)
    {
        uint8_t uart_buffer[UART_MAX_DATA_LEN];
        
        /* Check User SFlash, update the data storage format if necessary. */
        ConfigRW_CheckVersion();
        
        /* Start UART Configuration Service and Eddystone-URL Configuration Service. */
        UartCmd_Init(uart_buffer);
        EddystoneURL_Config_Init();
        
        /* Keep processing UART and BLE events. */
        while(true)
        {
            CyBle_ProcessEvents();
            /* If user typed EXIT command. */
            if(UartCmd_ProcessInput() == RESULT_EXIT)
            {
                break;
            }
            
            /* If the mode is EDTest, and GATT connection is terminated. */
            if((pBeaconConfig->mode == ADV_MODE_EDDYSTONETEST) && gatt_disconnect_flag)
            {
                beacon_state = BEACON_START;    /* skip the wait process */
                break;
            }
        }
        
        CyBle_Stop();
        UART_Stop();
    }
    
    ReadAndApplyConfig();
    
    /* Change advertisement settings from default connectable to non-connectable. */
    cyBle_discoveryParam.advType     = CYBLE_GAPP_NON_CONNECTABLE_UNDIRECTED_ADV;
    cyBle_discoveryModeInfo.advTo    = CYBLE_ADV_NO_TIMEOUT;
    cyBle_discoveryModeInfo.discMode = CYBLE_GAPP_NONE_DISC_BROADCAST_MODE;
    cyBle_discoveryData.advData[ADV_DATA1_FLAGS_INDEX] = ADV_DATA1_FLAGS_NON_CONNECTABLE;
    
    CyBle_Start((CYBLE_CALLBACK_T)BLE_AppEventHandler);
    
    for(;;)
    {
        CyBle_ProcessEvents(); /* BLE stack processing state machine interface */
    
        LowPowerSleep();
        
        /* If restartadvertisement flag is set, which means it's the first time cpu goes here. */
        if(restartadvertisement)
        {
            restartadvertisement = false;
            
            CySysWdtUnlock();
            
            CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
            
            CySysWdtWriteMode(I2C_COUNTER, CY_SYS_WDT_MODE_INT);
            
            CySysWdtWriteClearOnMatch(I2C_COUNTER, COUNTER_ENABLE);
            
#if I2C_SENSOR_ENABLE
            /* If I2C sensor is enabled and sensor setting is on. */
            if(Sensor_Flag)
            {
                CySysWdtWriteMatch(I2C_COUNTER, I2C_COUNT_PERIOD_10MS);
            }
            else
#endif
            /* If I2C sensor is disabled or sensor setting is off. */
            {
                CySysWdtWriteMatch(I2C_COUNTER, I2C_COUNT_PERIOD_10MS);
            }
            
            CySysWdtEnable(CY_SYS_WDT_COUNTER1_MASK);
            
            CySysWdtLock();
        }
        
#if I2C_SENSOR_ENABLE
        /* If I2C sensor is enabled and sensor setting is on. */
        if(Sensor_Flag)
        {
            /* If I2C communication is needed, wakeup I2C module. */
            if(i2c_state <= I2C_START_ADV)
            {
                I2CM_Wakeup();
                
                ProcessI2CEvents();
                
                I2CM_Sleep();       /* I2C communication is complete after invoking UpdateWdtCounter1.
                                       Put I2CM to sleep mode immediately to save power. */
            }
            else
            {
                ProcessI2CEvents();
            }
        }
        else
#endif
        /* If I2C sensor is disabled or sensor setting is off. */
        {
            ProcessBeaconEvents();
        }
    }
}

#if I2C_SENSOR_ENABLE
/******************************************************************************
* Function Name: ProcessI2CEvents
*******************************************************************************
*
* Summary:
*   This function handles I2C events according to current state and update the 
*   state value.
*
* Parameters:
*   None
*
* Return:
*   None
*
******************************************************************************/
void ProcessI2CEvents(void)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    
    switch(i2c_state)
    {
        /* Initialise I2CM and the sensor */
        case I2C_START:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                I2CM_Start();

                I2C_buffer[SI7020_WRITE_REG_INDEX_CMD]  = SI7020_WRITE_USER_REG;
                I2C_buffer[SI7020_WRITE_REG_INDEX_DATA] = USER_REG_SETTING;
                I2CM_I2CMasterWriteBuf(SI7020_SLAVE_ADDR, 
                                       I2C_buffer, 
                                       SI7020_WRITE_REG_SEND_LEN, 
                                       I2CM_I2C_MODE_COMPLETE_XFER);

                UpdateWdtCounter1(I2C_COUNT_PERIOD_20MS);
                
                /* Set next I2C state */
                i2c_state = I2C_READ_HUMIDITY_SEND_CMD;
            }
            break;
        
        /* Perform "Measure Relative Humidity" command to Si7020 */
        case I2C_READ_HUMIDITY_SEND_CMD:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                I2CM_I2CMasterClearStatus();
                
                /* Request RH data from I2C Sensor */
                I2C_buffer[I2C_BUFFER_HUM_OFFSET_CMD] = SI7020_MEASURE_RH;
                I2CM_I2CMasterWriteBuf(SI7020_SLAVE_ADDR, 
                                       &I2C_buffer[I2C_BUFFER_HUM_OFFSET_CMD], 
                                       SI7020_MEASURE_RH_SEND_LEN, 
                                       I2CM_I2C_MODE_COMPLETE_XFER);
                
                UpdateWdtCounter1(I2C_COUNT_PERIOD_20MS);
                
                /* Set next I2C state */
                i2c_state = I2C_READ_HUMIDITY_RECV_DATA;
            }
            break;
        
        /* Read back "Measure Relative Humidity" result from Si7020 */
        case I2C_READ_HUMIDITY_RECV_DATA:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                /* Read RH data from I2C Sensor */
                I2CM_I2CMasterReadBuf(SI7020_SLAVE_ADDR, 
                                      &I2C_buffer[I2C_BUFFER_HUM_OFFSET_DATA], 
                                      SI7020_MEASURE_RH_RECV_LEN, 
                                      I2CM_I2C_MODE_COMPLETE_XFER);
                
                UpdateWdtCounter1(I2C_COUNT_PERIOD_20MS);
                
                /* Set next I2C state */
                i2c_state = I2C_READ_TEMP_SEND_CMD;
            }
            break;
        
        /* Perform "Read Temperature Value" command to Si7020 */
        case I2C_READ_TEMP_SEND_CMD:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                I2CM_I2CMasterClearStatus();
                
                /* Request Temperature data from I2C Sensor */
                I2C_buffer[I2C_BUFFER_TEM_OFFSET_CMD] = SI7020_READ_TEMP;
                I2CM_I2CMasterWriteBuf(SI7020_SLAVE_ADDR, 
                                       &I2C_buffer[I2C_BUFFER_TEM_OFFSET_DATA], 
                                       SI7020_READ_TEMP_SEND_LEN, 
                                       I2CM_I2C_MODE_COMPLETE_XFER);

                UpdateWdtCounter1(I2C_COUNT_PERIOD_20MS);
                
                /* Set next I2C state */
                i2c_state = I2C_READ_TEMP_RECV_DATA;
            }
            break;
        
        /* Read back "Read Temperature Value" result from Si7020 */
        case I2C_READ_TEMP_RECV_DATA:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                /* Read Temperature data from I2C Sensor */
                I2CM_I2CMasterReadBuf(SI7020_SLAVE_ADDR, 
                                      &I2C_buffer[I2C_BUFFER_TEM_OFFSET_DATA], 
                                      SI7020_READ_TEMP_RECV_LEN, 
                                      I2CM_I2C_MODE_COMPLETE_XFER);
                
                UpdateWdtCounter1(I2C_COUNT_PERIOD_20MS);
                
                /* Set next I2C state */
                i2c_state = I2C_START_ADV;
            }
            break;
        
        /* Start BLEBeacon or Eddystone-UID Advertisement */
        case I2C_START_ADV:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                I2CM_I2CMasterClearStatus();
                
                /* If the interval is 0, don't advertise any data. */
                if(CFG_interval == 0)
                {
                    /* Set next I2C state */
                    i2c_state = I2C_WAIT;
                    
                    UpdateWdtCounter1(I2C_COUNT_PERIOD_1S);
                    
                    break;
                }
                /* If BLEBeacon mode is chosen, prepare the data according to BLEBeacon format. */
                else if(pCFG->mode == ADV_MODE_BLEBEACON)
                {
                     /* Update RH index of ADV packet with a new value */
                    cyBle_discoveryData.advData[ADDR_HUM_OFFSET] = I2C_buffer[I2C_BUFFER_HUM_OFFSET_DATA];
                    
                     /* Update Temperature index of ADV packet with a new value */
                    cyBle_discoveryData.advData[ADDR_TEM_OFFSET] = I2C_buffer[I2C_BUFFER_TEM_OFFSET_DATA];
                    
                    /* Set the next I2C state. */
                    i2c_state = I2C_STOP_ADV;
                }
                /* Eddystone mode is chosen, prepare the data according to Eddystone format. */
                else
                {
                    ES_ADV_CNT++;
                    ES_SEC_CNT_ms += BEACON_PERIOD_SENS_ON_1FRAME;
                    
                    /* Look for the frame which is advertised first. */
                    /* Eddystone-UID is on. */
                    if(pCFG->frame.UIDisEnabled)
                    {
                        UpdateAdData3(ES_FRAME_TYPE_UID);
                        
                        /* Set the next I2C state. */
                        /* Eddystone-URL is on. */
                        if(pCFG->frame.URLisEnabled)
                        {
                            i2c_state = I2C_ADV_URL;
                        }
                        /* Eddystone-TLM is on. */
                        else if(pCFG->frame.TLMisEnabled)
                        {
                            i2c_state = I2C_ADV_TLM;
                        }
                        /* Only Eddystone-UID is on, stop advertising next time. */
                        else
                        {
                            i2c_state = I2C_STOP_ADV;
                        }
                    }
                    /* Eddystone-UID is off, Eddystone-URL is on. */
                    else if(pCFG->frame.URLisEnabled)
                    {
                        UpdateAdData3(ES_FRAME_TYPE_URL);
                        
                        /* Set the next I2C state. */
                        /* Eddystone-TLM is on. */
                        if(pCFG->frame.TLMisEnabled)
                        {
                            i2c_state = I2C_ADV_TLM;
                        }
                        /* Only Eddystone-URL is on, stop advertising next time. */
                        else
                        {
                            i2c_state = I2C_STOP_ADV;
                        }
                    }
                    /* Only Eddystone-TLM is on. */
                    else
                    {
                        UpdateAdData3(ES_FRAME_TYPE_TLM);
                        
                        /* Set next I2C state */
                        i2c_state = I2C_STOP_ADV;
                    }
                }
                
                apiResult = Beacon_GappStartAdvertisement(CFG_interval);
                
                /* We don't need WDT counter to wake the system. BLESS will do it. */
                CySysWdtUnlock();
                CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);
                CySysWdtLock();
            }
            break;
        
        /* Advertise Eddystone-URL frame. */
        case I2C_ADV_URL:
            /* Handle the case if BLESS finishes Tx of the last frame. */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                ES_ADV_CNT++;
                ES_SEC_CNT_ms += CFG_interval;
                
                UpdateAdData3(ES_FRAME_TYPE_URL);
                
                /* Set the next I2C state. */
                /* Eddystone-TLM is on. */
                if(pCFG->frame.TLMisEnabled)
                {
                    i2c_state = I2C_ADV_TLM;
                }
                /* Eddystone-TLM is off, stop advertising next time. */
                else
                {
                    i2c_state = I2C_STOP_ADV;
                }
                
                apiResult = CyBle_GapUpdateAdvData(&cyBle_discoveryData, &cyBle_scanRspData);
            }
            break;
        
        /* Advertise Eddystone-TLM frame. */
        case I2C_ADV_TLM:
            /* Handle the case if BLESS finishes Tx of the last frame. */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                ES_ADV_CNT++;
                ES_SEC_CNT_ms += CFG_interval;
                
                UpdateAdData3(ES_FRAME_TYPE_TLM);
                
                /* Set next I2C state */
                i2c_state = I2C_STOP_ADV;
                
                apiResult = CyBle_GapUpdateAdvData(&cyBle_discoveryData, &cyBle_scanRspData);
            }
            break;
        
        /* Stop advertisement. */
        case I2C_STOP_ADV:
            /* Handle the case if BLESS finishes Tx of the last frame */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                /* Set next I2C state */
                i2c_state = I2C_WAIT;
            
                CyBle_GappStopAdvertisement();
                
                UpdateWdtCounter1(I2C_COUNT_PERIOD_1S);
            }
            break;
        
        /* Wait until the solar cell gathers enough power. */
        case I2C_WAIT:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                /* Set next I2C state */
                i2c_state = I2C_READ_HUMIDITY_SEND_CMD;
            }
            break;
            
        default:
            break;
    }
    
    /* clear wdt_trigger_on_flag if it's set */
    if(wdt_trigger_on_flag)
    {
        wdt_trigger_on_flag = false;
    }
    
    /* If fails to start advertisement or update the advertised data, halt the processor. */
    if(apiResult != CYBLE_ERROR_OK)
    {
        CYASSERT(0);
    }
}
#endif

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
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    
    switch(beacon_state)
    {
        /* Wait until the solar cell gets enough power. */
        case BEACON_WAIT:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                wdt_trigger_on_flag = false;
                
                /* Set next beacon state */
                beacon_state = BEACON_START;
            }
            break;
        
        /* Start advertisement. */
        case BEACON_START:
            /* Handle the case if I2C_COUNTER triggers a new interrupt. */
            if(wdt_trigger_on_flag)
            {
                wdt_trigger_on_flag = false;
                
                /* Stop I2C_COUNTER, and start advertisement */
                CySysWdtUnlock(); /* Unlock the WDT registers for modification */
                CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);
                CySysWdtLock();
                
                /* The default next situation is, updating advertised data isn't needed. */
                beacon_state = BEACON_RUN;
                
                /* If the interval is 0, let the system keep sleeping without any wakeup source, 
                 * so that there's no advertisement. */
                if(CFG_interval == 0)
                {
                    return;
                }
                
                /* If Eddystone mode is chosen, update advertised data. */
                if(pCFG->mode == ADV_MODE_EDDYSTONE)
                {
                    ES_ADV_CNT++;
                    ES_SEC_CNT_ms += CFG_interval;
                    
                    /* Look for the frame which is advertised first. */
                    /* Eddystone-UID is on. */
                    if(pCFG->frame.UIDisEnabled)
                    {
                        UpdateAdData3(ES_FRAME_TYPE_UID);
                        
                        /* Set next beacon state */
                        /* Eddystone-URL is on. */
                        if(pCFG->frame.URLisEnabled)
                        {
                            beacon_state = BEACON_URL;
                        }
                        /* Eddystone-TLM is on. */
                        else if(pCFG->frame.TLMisEnabled)
                        {
                            beacon_state = BEACON_TLM;
                        }
                        /* Only Eddystone-UID is on. */
                        else
                        {
                            /* beacon_state remains BEACON_RUN. */
                        }
                    }
                    /* Eddystone-UID is off, Eddystone-URL is on. */
                    else if(pCFG->frame.URLisEnabled)
                    {
                        UpdateAdData3(ES_FRAME_TYPE_URL);
                        
                        /* Set next beacon state */
                        /* Eddystone-TLM is on. */
                        if(pCFG->frame.TLMisEnabled)
                        {
                            beacon_state = BEACON_TLM;
                        }
                        /* Only Eddystone-URL is on. */
                        else
                        {
                            /* beacon_state remains BEACON_RUN. */
                        }
                    }
                    /* Only Eddystone-TLM is on. */
                    else
                    {
                        UpdateAdData3(ES_FRAME_TYPE_TLM);
                        
                        /* Set next beacon state */
                        beacon_state = BEACON_TLMCNT;
                    }
                }
                /* If EDTest mode is chosen, ignore EDFRAME setting. */
                else if(pCFG->mode == ADV_MODE_EDDYSTONETEST)
                {
                    UpdateAdData3(ES_FRAME_TYPE_URL);
                }
                
                apiResult = Beacon_GappStartAdvertisement(CFG_interval);
            }
            break;
            
        /* Update advertisement data to Eddystone-UID frame. */
        case BEACON_UID:
            /* Handle the case if BLESS finishes Tx of the last frame */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                ES_ADV_CNT++;
                ES_SEC_CNT_ms += CFG_interval;
                
                UpdateAdData3(ES_FRAME_TYPE_UID);
                
                apiResult = CyBle_GapUpdateAdvData(&cyBle_discoveryData, &cyBle_scanRspData);
                
                /* Set next beacon state */
                /* Check whether the next frame is Eddystone-URL. */
                if(pCFG->frame.URLisEnabled)
                {
                    beacon_state = BEACON_URL;
                }
                /* The next frame is Eddystone-TLM. */
                else
                {
                    beacon_state = BEACON_TLM;
                }
            }
            break;
            
        /* Update advertisement data to Eddystone-URL frame. */
        case BEACON_URL:
            /* Handle the case if BLESS finishes Tx of the last frame */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                ES_ADV_CNT++;
                ES_SEC_CNT_ms += CFG_interval;
                
                UpdateAdData3(ES_FRAME_TYPE_URL);
                
                apiResult = CyBle_GapUpdateAdvData(&cyBle_discoveryData, &cyBle_scanRspData);
                
                /* Set next beacon state */
                /* Check whether the next frame is Eddystone-TLM. */
                if(pCFG->frame.TLMisEnabled)
                {
                    beacon_state = BEACON_TLM;
                }
                /* The next frame is Eddystone-UID. */
                else
                {
                    beacon_state = BEACON_UID;
                }
            }
            break;
            
        /* Update advertisement data to Eddystone-TLM frame. */
        case BEACON_TLM:
            /* Handle the case if BLESS finishes Tx of the last frame */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                ES_ADV_CNT++;
                ES_SEC_CNT_ms += CFG_interval;
                
                UpdateAdData3(ES_FRAME_TYPE_TLM);
                
                apiResult = CyBle_GapUpdateAdvData(&cyBle_discoveryData, &cyBle_scanRspData);
                
                /* Set next beacon state */
                /* Check whether the next frame is Eddystone-UID. */
                if(pCFG->frame.UIDisEnabled)
                {
                    beacon_state = BEACON_UID;
                }
                /* The next frame is Eddystone-URL. */
                else
                {
                    beacon_state = BEACON_URL;
                }
            }
            break;
            
        /* Only TLM frame is enabled, update ADV_CNT and SEC_CNT. */
        case BEACON_TLMCNT:
            /* Handle the case if BLESS finishes Tx of the last frame */
            if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                ES_ADV_CNT++;
                ES_SEC_CNT_ms += CFG_interval;
                
                /* Update ADV_CNT and SEC_CNT. */
                UpdateAdData3(ES_FRAME_TYPE_TLM);
                
                apiResult = CyBle_GapUpdateAdvData(&cyBle_discoveryData, &cyBle_scanRspData);
            }
            break;
            
        default:
            break;
    }
    
    /* If fails to start or update the advertisement, halt the processor. */
    if(apiResult != CYBLE_ERROR_OK)
    {
        CYASSERT(0);
    }
}

/******************************************************************************
* Function Name: ReadAndApplyConfig
*******************************************************************************
*
* Summary:
*   Read configuration in SFlash and apply. Prepare the data to be advertised.
*
* Parameters:
*   None
*
* Return:
*   None
*******************************************************************************/
void ReadAndApplyConfig(void)
{
    /* If UserSFlash is not empty and the storage format isn't older than the firmware 
     * version, use configuration in UserSFlash. */
    if( (pBeaconConfig->DataFlag_End   == SFLASH_DATA_FLAG_VALID) && \
        (pBeaconConfig->DataFlag_Start == SFLASH_DATA_FLAG_VALID) && \
        (pBeaconConfig->version_major_minor1 >= FW_VERSION)          )
    {
        pCFG = pBeaconConfig;
    }
    /* Otherwise default configuration will be used. */
    else
    {
        pCFG = (BeaconConfig *)&DefaultBeaconConfig;
    }
    
    /* If BLEBeacon mode is chosen. */
    if(pCFG->mode == ADV_MODE_BLEBEACON)
    {
        CFG_interval = pCFG->itrvl;
        CFG_txpwr    = pCFG->txpwr;
#if (I2C_SENSOR_ENABLE)
        Sensor_Flag  = pCFG->sensor;
#endif
        
        /* Prepare BLEBeacon data */
        cyBle_discoveryData.advData[BLEBEACON_LENGTH2_OFFSET]     = BLEBEACON_LENGTH2_VALUE;
        cyBle_discoveryData.advData[BLEBEACON_AD_TYPE2_OFFSET]    = BLEBEACON_AD_TYPE2_VALUE;
        cyBle_discoveryData.advData[BLEBEACON_DEVICE_TYPE_OFFSET] = BLEBEACON_DEVICE_TYPE_VALUE;
        cyBle_discoveryData.advData[BLEBEACON_LENGTH3_OFFSET]     = BLEBEACON_LENGTH3_VALUE;
        
        cyBle_discoveryData.advDataLen = BLEBEACON_ADV_LENGTH;
        
        /* Set UUID */
        memcpy(&(cyBle_discoveryData.advData[ADDR_UUID_OFFSET]), pCFG->uuid, sizeof(pCFG->uuid));

        /* Apply Little Endian to COID, send Lower Byte first. */
        cyBle_discoveryData.advData[ADDR_COID_OFFSET]     = pCFG->comID & 0xFF;
        cyBle_discoveryData.advData[ADDR_COID_OFFSET + 1] = pCFG->comID >> 8;

        /* Apply Big Endian to MAJOR, send Higher Byte first. */
        cyBle_discoveryData.advData[ADDR_MAJOR_OFFSET]     = pCFG->major >> 8;
        cyBle_discoveryData.advData[ADDR_MAJOR_OFFSET + 1] = pCFG->major & 0xFF;
        
        /* Apply Big Endian to MINOR, send Higher Byte first. */
        cyBle_discoveryData.advData[ADDR_MINOR_OFFSET]     = pCFG->minor >> 8;
        cyBle_discoveryData.advData[ADDR_MINOR_OFFSET + 1] = pCFG->minor & 0xFF;
        
        /* Set RSSI */
        cyBle_discoveryData.advData[ADDR_RSSI_OFFSET] = pCFG->rssi;          
    }
    /* Eddystone or EDTest mode is chosen. */
    else
    {
        CFG_interval = pCFG->URLPeriod;
        CFG_txpwr    = pCFG->TXPowerLevels[pCFG->TXPowerMode];
#if (I2C_SENSOR_ENABLE)
        /* If the mode is EDTest, sensor flag is forced to be off. */
        Sensor_Flag  = (pCFG->mode == ADV_MODE_EDDYSTONETEST) ? false : pCFG->sensor;
#endif
        
        /* Service Solicitation data */
        cyBle_discoveryData.advData[ES_ADV_SVC_UUID_INDEX_LEN]  = ES_ADV_SVC_UUID_LENGTH - SIZE_LEN_IN_BLE_AD_DATA;
        cyBle_discoveryData.advData[ES_ADV_SVC_UUID_INDEX_TYPE] = ES_SVC_SOLICITATION;
        cyBle_discoveryData.advData[ES_ADV_SVC_UUID_INDEX_LSB]  = ES_SVC_LSB;
        cyBle_discoveryData.advData[ES_ADV_SVC_UUID_INDEX_MSB]  = ES_SVC_MSB;
        
        /* Eddystone-UID buffer */
        UID_FrameBuffer[ES_ADV_TXPL_INDEX_INFRAME] = pCFG->Adv_TXPowerLevels[pCFG->TXPowerMode];
        memcpy(&(UID_FrameBuffer[ES_NID_INDEX_INFRAME]), pCFG->nid, NID_BYTES);
        memcpy(&(UID_FrameBuffer[ES_BID_INDEX_INFRAME]), pCFG->bid, BID_BYTES);
        
        /* Eddystone-URL buffer */
        URL_FrameBuffer[ES_FRAME_TYPE_INDEX_INFRAME] = ES_FRAME_TYPE_URL | (pCFG->URL_Flags & 0x0F);
        URL_FrameBuffer[ES_ADV_TXPL_INDEX_INFRAME]   = pCFG->Adv_TXPowerLevels[pCFG->TXPowerMode];
        
        URL_FrameBuffer[INDEX_LEN_IN_BLE_AD_DATA] = ES_URI_DATA_INDEX_INFRAME + pCFG->URI_Length - SIZE_LEN_IN_BLE_AD_DATA;
        memcpy(&(URL_FrameBuffer[ES_URI_DATA_INDEX_INFRAME]), pCFG->URI_Data, pCFG->URI_Length);
    }
    
    return;
}

/******************************************************************************
* Function Name: UpdateTxPowerLeverl
*******************************************************************************
*
* Summary:
*   Set BLE Tx Power level to the expected value.
*
* Parameters:
*   NewLevel - New Tx Power level 
*
* Return:
*   None
*
******************************************************************************/
void UpdateTxPowerLeverl(int8_t NewLevel)
{
    CYBLE_BLESS_PWR_IN_DB_T t_pwr;
    CYBLE_BLESS_PWR_LVL_T param_pwr;
    
    /* Valid values are -18, -12, -6, -3, -2, -1, 0 and 3 dBm. */
    if(NewLevel == TXPWR_ACCEPTED_VALUE_NEG_18)
    {
        param_pwr = CYBLE_LL_PWR_LVL_NEG_18_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_NEG_12)
    {
        param_pwr = CYBLE_LL_PWR_LVL_NEG_12_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_NEG_6)
    {
        param_pwr = CYBLE_LL_PWR_LVL_NEG_6_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_NEG_3)
    {
        param_pwr = CYBLE_LL_PWR_LVL_NEG_3_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_NEG_2)
    {
        param_pwr = CYBLE_LL_PWR_LVL_NEG_2_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_NEG_1)
    {
        param_pwr = CYBLE_LL_PWR_LVL_NEG_1_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_0)
    {
        param_pwr = CYBLE_LL_PWR_LVL_0_DBM;
    }
    else if(NewLevel == TXPWR_ACCEPTED_VALUE_3)
    {
        param_pwr = CYBLE_LL_PWR_LVL_3_DBM;
    }
    else
    {
        param_pwr = CYBLE_LL_PWR_LVL_MAX;
    }
    t_pwr.blePwrLevelInDbm = param_pwr;
    t_pwr.bleSsChId = CYBLE_LL_ADV_CH_TYPE;
    CyBle_SetTxPowerLevel(&t_pwr);
    
    return;
}

/******************************************************************************
* Function Name: Beacon_GappStartAdvertisement
*******************************************************************************
*
* Summary:
*   This function is used to start the advertisement using the given advertising 
*   interval.
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
CYBLE_API_RESULT_T Beacon_GappStartAdvertisement(uint32_t advertisingInterval)
{
    uint16_t RegisterIntervalValue;
    
    RegisterIntervalValue = CALC_REG_INTERVAL(advertisingInterval);
    /* Set advertising broadcast interval*/
    cyBle_discoveryModeInfo.advParam->advIntvMin = RegisterIntervalValue;
    cyBle_discoveryModeInfo.advParam->advIntvMax = RegisterIntervalValue;

    /* Start advertising */
    return  CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_CUSTOM);
}

#if I2C_SENSOR_ENABLE
/******************************************************************************
* Function Name: UpdateWdtCounter1
*******************************************************************************
*
* Summary:
*   This function update the setting of WDT counter 1 and restart it.
*
* Parameters:
*   match - The value to be used to match against the counter.
*
* Return:
*   None
*
******************************************************************************/
static void UpdateWdtCounter1(uint32_t match)
{
    CySysWdtUnlock();
    CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);
    CySysWdtWriteMode(I2C_COUNTER, CY_SYS_WDT_MODE_INT);
    CySysWdtWriteClearOnMatch(I2C_COUNTER, COUNTER_ENABLE);
    CySysWdtWriteMatch(I2C_COUNTER, match);
    CySysWdtEnable(CY_SYS_WDT_COUNTER1_MASK);
    CySysWdtLock();            
}
#endif

/******************************************************************************
* Function Name: UpdateAdData3
*******************************************************************************
*
* Summary:
*   This function is used to update the advertisement data buffer, including 
*   humidity and temperature if SENSOR is ON.
*
* Parameters:
*   NewEdFrame - The new Eddystone frame type. The valid value is ES_FRAME_TYPE_UID,
*                ES_FRAME_TYPE_URL or ES_FRAME_TYPE_TLM.
*
* Return:
*   None
******************************************************************************/
void UpdateAdData3(uint8_t NewEdFrame)
{
    switch(NewEdFrame)
    {
        /* Update cyBle_discoveryData to be Eddystone-UID frame. */
        case ES_FRAME_TYPE_UID:
            memcpy(&(cyBle_discoveryData.advData[ES_FRAME_OFFSET]), 
                   UID_FrameBuffer, 
                   sizeof(UID_FrameBuffer));
            
            /* ADV packet length */
            cyBle_discoveryData.advDataLen = ES_ADV_FLAGS_LENGTH + 
                                             ES_ADV_SVC_UUID_LENGTH + 
                                             ES_UID_FRAME_ARRAY_LENGTH;
            break;
        
        /* Update cyBle_discoveryData to be Eddystone-URL frame. */
        case ES_FRAME_TYPE_URL:
            memcpy(&(cyBle_discoveryData.advData[ES_FRAME_OFFSET]), 
                   URL_FrameBuffer, 
                   URL_FrameBuffer[INDEX_LEN_IN_BLE_AD_DATA] + SIZE_LEN_IN_BLE_AD_DATA);
        
            /* ADV packet length */
            cyBle_discoveryData.advDataLen = ES_ADV_FLAGS_LENGTH +    \
                                             ES_ADV_SVC_UUID_LENGTH + \
                                             URL_FrameBuffer[INDEX_LEN_IN_BLE_AD_DATA] + SIZE_LEN_IN_BLE_AD_DATA;
            break;
        
        /* Update cyBle_discoveryData to be Eddystone-TLM frame. */
        case ES_FRAME_TYPE_TLM:
            memcpy(&(cyBle_discoveryData.advData[ES_FRAME_OFFSET]), 
                   DefaultTLMframe, 
                   sizeof(DefaultTLMframe));
            
            cyBle_discoveryData.advData[ES_TLM_INDEX_ADV_CNT]     = GET_BYTE1_BIG_ENDIAN(ES_ADV_CNT);
            cyBle_discoveryData.advData[ES_TLM_INDEX_ADV_CNT + 1] = GET_BYTE2_BIG_ENDIAN(ES_ADV_CNT);
            cyBle_discoveryData.advData[ES_TLM_INDEX_ADV_CNT + 2] = GET_BYTE3_BIG_ENDIAN(ES_ADV_CNT);
            cyBle_discoveryData.advData[ES_TLM_INDEX_ADV_CNT + 3] = GET_BYTE4_BIG_ENDIAN(ES_ADV_CNT);
            
            ES_SEC_CNT   += ES_SEC_CNT_ms / RATIO_0_1SEC_TO_MS;
            ES_SEC_CNT_ms = ES_SEC_CNT_ms % RATIO_0_1SEC_TO_MS;
            
            cyBle_discoveryData.advData[ES_TLM_INDEX_SEC_CNT]     = GET_BYTE1_BIG_ENDIAN(ES_SEC_CNT);
            cyBle_discoveryData.advData[ES_TLM_INDEX_SEC_CNT + 1] = GET_BYTE2_BIG_ENDIAN(ES_SEC_CNT);
            cyBle_discoveryData.advData[ES_TLM_INDEX_SEC_CNT + 2] = GET_BYTE3_BIG_ENDIAN(ES_SEC_CNT);
            cyBle_discoveryData.advData[ES_TLM_INDEX_SEC_CNT + 3] = GET_BYTE4_BIG_ENDIAN(ES_SEC_CNT);
            
            /* ADV packet length */
            cyBle_discoveryData.advDataLen = ES_ADV_FLAGS_LENGTH + 
                                             ES_ADV_SVC_UUID_LENGTH + 
                                             ES_TLM_FRAME_ARRAY_LENGTH;
            
#if I2C_SENSOR_ENABLE
            /* If I2C sensor is enabled and sensor setting is on. */
            if(Sensor_Flag)
            {
                uint16_t temp;
                int16_t temperature;
                
                /* Calculate Temperature. */
                temp = (I2C_buffer[I2C_BUFFER_TEM_OFFSET_DATA] << 8) | 
                        I2C_buffer[I2C_BUFFER_TEM_OFFSET_DATA + 1];
                temperature = CALC_TEMP_100(temp);
                temperature = FLOAT100_TO_FLOAT256(temperature);
                
                cyBle_discoveryData.advData[ES_TLM_INDEX_TEMP_MSB] = temperature >> 8;
                cyBle_discoveryData.advData[ES_TLM_INDEX_TEMP_LSB] = temperature & 0xFF;
            }
#endif
            break;
        
        default:
            break;
    }
}

/* [] END OF FILE */
