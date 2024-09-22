/******************************************************************************
* Project Name      : Debug_Board
* File Name         : main.c
* Version           : CYALKIT-E02 Sample Firmware, Version 1.2.00
* Device Used       : CYBL10162-56LQXI
* Software Used     : PSoC Creator 3.3 CP3
* Compiler          : ARM GCC 4.9.3
* Related Hardware  : BLE-USB_Bridge and Debug Board
*
********************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
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

#include "main.h"

/*****************************************************************************
* Global Variable Definition
*****************************************************************************/
static int8_t ledBlinkCount = 0;
static REPORT_DATA_BUFFER_T g_rptBuf[MAX_BUFFER_NUMBER];
static REPORT_DATA_BUFFER_T * g_LASP = NULL; //loop array start pos
static REPORT_DATA_BUFFER_T * g_LAEP = NULL; //loop array end pos
static uint8_t g_uartCtlFlag = '0';          /* Flag for controlling filter conditions*/
                                             /* '0' accept all bluetooth data*/
                                             /* '1' accept qualified bluetooth data*/

/*******************************************************************************
* Function Name: Init_Ring_Buffer
********************************************************************************
*
* Summary:
*  This function initializes the ring buffer
*
* Parameters:
*  NONE
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  NONE
*
* Note:
*
*******************************************************************************/
void Init_Ring_Buffer()
{
    int i = 0;

    for (i = 0; i < MAX_BUFFER_NUMBER - 1; i++)
    {
        g_rptBuf[i].pNext = &g_rptBuf[i + 1];
        g_rptBuf[i].dataStatus = CYBLE_BUFFER_IDLE;
    }
    g_rptBuf[MAX_BUFFER_NUMBER - 1].pNext = &g_rptBuf[0];
    g_rptBuf[MAX_BUFFER_NUMBER - 1].dataStatus = CYBLE_BUFFER_IDLE;
    g_LASP = &g_rptBuf[0];
    g_LAEP = &g_rptBuf[0];
}

/*******************************************************************************
* Function Name: Insert_New_Data
********************************************************************************
*
* Summary:
*  This function is used to insert a new bluetooth data into the ring buffer
*
* Parameters:
*  CYBLE_GAPC_ADV_REPORT_T * pReport   : received new bluetooth data, length and rssi.
*  uint16_t                  dataType  : type of received new bluetooth data,
*  bool                      isID_Match: flag which shows if received data is CY's device.
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  NONE
*
* Note:
*  interrupt is not allowed
*
*******************************************************************************/
void Insert_New_Data(CYBLE_GAPC_ADV_REPORT_T *pReport, uint16_t dataType, bool isID_Match)
{
    uint8_t lock;
    int32_t i;

    lock = CyEnterCriticalSection();

    /*To ensure that the pointer of ring buffer is not NULL and param btData is valid*/
    if (NULL == g_LAEP || NULL == g_LASP || NULL == pReport)
    {
        CyExitCriticalSection(lock);
        return;
    }
    
    /* Current buffer is not idle, it means that 
     * ring buffer is full, overwrite the first data*/
    if (CYBLE_BUFFER_IDLE != g_LAEP->dataStatus)
    {
        /* buffer is full, overwrite first data */
        if (g_LAEP == g_LASP)
        {
            g_LASP->dataStatus = CYBLE_BUFFER_IDLE;
            g_LASP             = g_LASP->pNext;
        }
        else
        {
            /* never gonna happen */
        }
    }

    if(dataType == DATA_FORMAT_V1_0_BLEBEACON)
    {
        /* 'Tx Power' value is overwritten because Rssi is needed by PMIC.exe. 
         * The tool assumes 'Tx Power' is a fixed value. */
        memcpy(g_LAEP->rptData.dataBuf, pReport->data, pReport->dataLen - 1);
        g_LAEP->rptData.dataBuf[BLE_ADV_MAX_LENGTH-2] = pReport->rssi;
        g_LAEP->dataLen = pReport->dataLen;
    }
    else
    {
        /* Set Magic Number */
        g_LAEP->dataLen = MAGIC_NUM_LENGTH + TYPE_LENGTH;
        g_LAEP->rptData.dataBuf[0] = 'C';
        g_LAEP->rptData.dataBuf[1] = 'Y';

        /* Bluetooth data exceeded the length limit */
        if (pReport->dataLen > BLE_ADV_MAX_LENGTH)
        {
            g_LAEP->rptData.dataBuf[TYPE_OFFSET] = DATA_FORMAT_V2_0_RAW & 0xFF;
            memcpy(&g_LAEP->rptData.dataBuf[g_LAEP->dataLen], pReport->data, BLE_ADV_MAX_LENGTH);
            g_LAEP->dataLen += BLE_ADV_MAX_LENGTH;
        }
        else if (dataType == DATA_FORMAT_V2_0_EDDYSTONE)
        {
            g_LAEP->rptData.dataBuf[TYPE_OFFSET] = DATA_FORMAT_V2_0_EDDYSTONE & 0xFF;

            for(i = 0; i < BLE_ADDRESS_LENGTH; i++)
            {
                g_LAEP->rptData.dataBuf[BLE_ADDRESS_OFFSET + i] = 
                    pReport->peerBdAddr[BLE_ADDRESS_LENGTH - 1 - i];
            }
            g_LAEP->rptData.dataBuf[RSSI_OFFSET] = (uint8_t)pReport->rssi;

            memcpy(&g_LAEP->rptData.dataBuf[BLE_ADV_OFFSET], pReport->data, pReport->dataLen);
            g_LAEP->dataLen += pReport->dataLen + BLE_ADDRESS_LENGTH + RSSI_LENGTH;
        }
        else
        {
            g_LAEP->rptData.dataBuf[TYPE_OFFSET] = DATA_FORMAT_V2_0_RAW & 0xFF;
            memcpy(&g_LAEP->rptData.dataBuf[g_LAEP->dataLen], pReport->data, pReport->dataLen);
            g_LAEP->dataLen += pReport->dataLen;
        }
    }
    
    g_LAEP->rptData.dataType = dataType;
    g_LAEP->rptData.flag = (isID_Match) ? DATA_FRAG_CY_DEVICE : 0;
    
    /* Set buffer data status flag to in use*/
    g_LAEP->dataStatus = CYBLE_BUFFER_INSERT;
    g_LAEP = g_LAEP->pNext;

    CyExitCriticalSection(lock);
}

/*******************************************************************************
* Function Name: Handle_Bluetooth_Data
********************************************************************************
*
* Summary:
*  This function is used to get bluetooth data from ring buffer 
*  and send to the PC by UART
*
* Parameters:
*  NONE
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  NONE
*
* Note:
*  interrupt is not allowed
*
*******************************************************************************/
void Handle_Bluetooth_Data()
{
    uint8_t lock;
    uint32_t i;
    static uint8_t sendBuffer[UART_SEND_BUFFER_SIZE];
    
    lock = CyEnterCriticalSection();

    /*To ensure that the pointer of ring buffer is not NULL*/
    if (NULL == g_LAEP || NULL == g_LASP)
    {
        CyExitCriticalSection(lock);
        return;
    }
    /* Ring buffer is empty */
    if (CYBLE_BUFFER_IDLE == g_LASP->dataStatus)
    {
        CyExitCriticalSection(lock);
        return;
    }
    /* Change one byte binary data to 2 bytes ASCII data. */
    for (i = 0; i < g_LASP->dataLen; i++)
    {
        sprintf((char *)(&sendBuffer[i * 2]), "%02X", g_LASP->rptData.dataBuf[i]);
    }

    sendBuffer[g_LASP->dataLen * 2]     = '\r';
    sendBuffer[g_LASP->dataLen * 2 + 1] = '\n';
    sendBuffer[g_LASP->dataLen * 2 + 2] = 0;

    /* Current buffer has been handled, set its status to idle,
     * and set start pointer as next buffer */
    g_LASP->dataStatus = CYBLE_BUFFER_IDLE;
    g_LASP = g_LASP->pNext;
    CyExitCriticalSection(lock);
    
    /* Send ASCII data to the serial port */
    UART_UartPutString((const char *)sendBuffer);
}


/*****************************************************************************
* Function Name: LED_Init()
******************************************************************************
* Summary:
* This function initializes the LED module
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* None
* 
* Note:
* None
*****************************************************************************/
void LED_Init(void)
{
    Prism_Led_Init();
    Prism_Led_Clock_Stop();
}

/*****************************************************************************
* Function Name: Led_On()
******************************************************************************
* Summary:
* This function is used to keep the LED at certain intensity
*
* Parameters:
* uint32_t intensity: value of the intensity
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* NONE
* 
* Note:
* Intensity should be set as less than the Period mention in the Prism_Led 
* component
*****************************************************************************/
void LED_On(uint32_t intensity)
{
    /*Set the drive mode of the led pin to open drain*/
    Led_Pin_SetDriveMode(Led_Pin_DM_OD_HI);
    Prism_Led_WriteCompare(intensity);
    Prism_Led_Clock_Start();
    Prism_Led_Start();
}

/*****************************************************************************
* Function Name: LED_Off()
******************************************************************************
* Summary:
* This function turns off the LED
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* NONE
* 
* Note:
* None
*****************************************************************************/
void LED_Off()
{
    /*Set the drive mode of the led pin to digital high impedance*/
    Led_Pin_SetDriveMode(Led_Pin_DM_DIG_HIZ);
    Prism_Led_Stop();
    Prism_Led_Clock_Stop();
}

/*******************************************************************************
* Function Name: LED_Flicker
********************************************************************************
*
* Summary:
*  This function is used to achieve the effect of flashing LED
*
* Parameters:
*  NONE
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  NONE
*
* Note:
* None
*******************************************************************************/
void LED_Flicker()
{
    static int32_t ledBlinkIntervalCount = 0;

    /* blink time is out */
    if (0 == ledBlinkCount)
    {
        --ledBlinkCount;

        /* Blink time is out Led should be closed */
        LED_Off();
        return;
    }

    /* Blink time is out */
    if (ledBlinkCount < 0)
    {
        return;
    }

    /* If blink count is not lower than 0, self-decrease */
    --ledBlinkCount;

    /* ledBlinkIntervalCount is circle between 0 - FLICKER_PERIOD,if it is
     * large than FLICKER_PERIOD, set the value of ledBlinkIntervalCount to 0*/
    if (ledBlinkIntervalCount >= FLICKER_PERIOD)
    {
        ledBlinkIntervalCount = 0;
    }
    /* 0 - LIGHT_INTERVAL is led turn on period*/
    if (ledBlinkIntervalCount < LIGHT_INTERVAL)
    {
        LED_On(DEVICE_LED_ON_INTENSITY);
    }
    /* LIGHT_INTERVAL - FLICKER_PERIOD is led turn off period*/
    else
    {
        LED_Off();
    }

    /*self-increase all the time */
    ++ledBlinkIntervalCount;
}

/*******************************************************************************
* Function Name: SysTickISRCallback
********************************************************************************
*
* Summary:
*  This API is called from SysTick timer interrupt handler to update the
*  millisecond counter.
*
*  it will check the waiting time of the received bluetooth data in the 
*  ring buffer and make sure it is not expired
*  
*  it will update the flashing state of LED
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SysTickISRCallback(void)
{
    REPORT_DATA_BUFFER_T * curPos = NULL;
    uint8_t lock;

    /* Achieve the effect of flashing LED */
    LED_Flicker();
    
    /*Interrupt is not allowed now*/
    lock = CyEnterCriticalSection();

    /*To ensure that the pointer of ring buffer is not NULL*/
    if (NULL ==g_LASP || NULL == g_LAEP)
    {
        CyExitCriticalSection(lock);
        return;
    }

    /* Update wait count of unprocessed buffer*/
    for (curPos = g_LASP; curPos != g_LAEP; curPos = curPos->pNext)
    {
        /* time is up*/
        if (curPos->dataStatus >= CYBLE_BUFFER_EXPIRED)
        {
            /* Drop it */
            g_LASP->dataStatus = CYBLE_BUFFER_IDLE;
            g_LASP = g_LASP->pNext;
        }
        else
        {
            curPos->dataStatus++;
        }
    }
    CyExitCriticalSection(lock);
}

/*******************************************************************************
* Function Name: eventHandler
********************************************************************************
*
* Summary:
*  This function handles events from the CYBLE component
*
* Parameters:  
*  uint32_t event:   Event from the CYBLE component
*  void* eventParam: A structure instance for corresponding event type. The 
*                     list of event structure is described in the component 
*                     data-sheet.
*
* Return: 
*  None
*
*******************************************************************************/
static void eventHandler(uint32_t event, void *eventParam)
{
    switch(event)
    {
        /* recv advertising data */
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
        {
            uint8_t getValue = 0;
            bool isID_Match = false;
            CYBLE_GAPC_ADV_REPORT_T * t_report = (CYBLE_GAPC_ADV_REPORT_T *)eventParam;

            /* get character from UART */
            getValue = UART_UartGetChar();

            /* If UART gets new data, and the data is not "new line", then update Control Flag. */
            if((getValue != 0) && (getValue != '\r') && (getValue != '\n'))
            {
                g_uartCtlFlag = getValue;
            }

            /* Make sure function parameter is not empty*/
            if (NULL ==t_report)
            {
                return;
            }

            /* Check if the received data is BLEBeacon format, and the UUID matches CY's UUID. */
            if((BLEBEACON_DATA2_LENGTH_VALUE1 == t_report->data[BLEBEACON_DATA2_LENGTH_OFFSET])   &&
                    (BLEBEACON_DATA2_TYPE_VALUE   == t_report->data[BLEBEACON_DATA2_TYPE_OFFSET])     &&
                    (BLEBEACON_DEVICE_TYPE_VALUE  == t_report->data[BLEBEACON_DEVICE_TYPE_OFFSET])    &&
                    (EXPECTED_UUID_BYTE1          == t_report->data[BLEBEACON_DATA2_UUID_OFFSET])     &&
                    (EXPECTED_UUID_BYTE2          == t_report->data[BLEBEACON_DATA2_UUID_OFFSET + 1]) &&
                    (EXPECTED_UUID_BYTE3          == t_report->data[BLEBEACON_DATA2_UUID_OFFSET + 2]) &&
                    (EXPECTED_UUID_BYTE4          == t_report->data[BLEBEACON_DATA2_UUID_OFFSET + 3]))
            {
                isID_Match = true;
                Insert_New_Data(t_report, DATA_FORMAT_V1_0_BLEBEACON, isID_Match);
            }
            else if(g_uartCtlFlag != CTRLFLAG_FILTER_ON)
            {
                uint16_t dataType;
    
                if(((BLEBEACON_DATA2_LENGTH_VALUE1 == t_report->data[BLEBEACON_DATA2_LENGTH_OFFSET])  ||
                    (BLEBEACON_DATA2_LENGTH_VALUE2 == t_report->data[BLEBEACON_DATA2_LENGTH_OFFSET])) &&
                        (BLEBEACON_DATA2_TYPE_VALUE  == t_report->data[BLEBEACON_DATA2_TYPE_OFFSET])  &&
                        (BLEBEACON_DEVICE_TYPE_VALUE == t_report->data[BLEBEACON_DEVICE_TYPE_OFFSET]))
                {
                    dataType = DATA_FORMAT_V1_0_BLEBEACON;
                }
                /* If the advertisement is Eddystone-UID/URL/TLM frame, 
                 * check whether the data matches the expected value. */
                else if((ES_SVC_SOLI_LEN_VALUE     == t_report->data[ES_SVC_SOLI_LEN_OFFSET])     &&
                        (ES_SVC_SOLI_TYPE_VALUE    == t_report->data[ES_SVC_SOLI_TYPE_OFFSET])    &&
                        (ES_SVC_SOLI_SVC_MSB_VALUE == t_report->data[ES_SVC_SOLI_SVC_MSB_OFFSET]) &&
                        (ES_SVC_SOLI_SVC_LSB_VALUE == t_report->data[ES_SVC_SOLI_SVC_LSB_OFFSET]) &&
                        (ES_SVC_SOLI_TYPE2_VALUE   == t_report->data[ES_SVC_SOLI_TYPE2_OFFSET])    &&
                        (ES_SVC_SOLI_SVC_MSB_VALUE == t_report->data[ES_SVC_SOLI_SVC2_MSB_OFFSET]) &&
                        (ES_SVC_SOLI_SVC_LSB_VALUE == t_report->data[ES_SVC_SOLI_SVC2_LSB_OFFSET]) )
                {
                    dataType = DATA_FORMAT_V2_0_EDDYSTONE;

                    if((ES_SVC_SOLI_FRAME_TYPE_UID == t_report->data[ES_SVC_SOLI_FRAME_TYPE_OFFSET]) &&
                            (EXPECTED_NID_BYTE1   == t_report->data[ES_SVC_SOLI_NID_OFFSET])   &&
                            (EXPECTED_NID_BYTE2   == t_report->data[ES_SVC_SOLI_NID_OFFSET+1]) &&
                            (EXPECTED_NID_BYTE3   == t_report->data[ES_SVC_SOLI_NID_OFFSET+2]) &&
                            (EXPECTED_NID_BYTE4   == t_report->data[ES_SVC_SOLI_NID_OFFSET+3]))
                    {
                        isID_Match = true;
                    }
                    else if((ES_SVC_SOLI_FRAME_TYPE_URL == t_report->data[ES_SVC_SOLI_FRAME_TYPE_OFFSET]) ||
                            (ES_SVC_SOLI_FRAME_TYPE_TLM == t_report->data[ES_SVC_SOLI_FRAME_TYPE_OFFSET]))
                    {
                        REPORT_DATA_T * data;
                        int32_t i;
                        for(i=0; i<MAX_BUFFER_NUMBER; i++)
                        {
                            /* Check whether the device is Eddystone-NID and has CY's NID. */
                            data = &g_rptBuf[i].rptData;
                            if((data->flag & DATA_FRAG_CY_DEVICE) != DATA_FRAG_CY_DEVICE)
                            {
                                continue;
                            }
                            /* Check whether the addr of received data is same as CY's device. */
                            if((data->dataBuf[BLE_ADDRESS_OFFSET + 5] == t_report->peerBdAddr[0]) &&
                                    (data->dataBuf[BLE_ADDRESS_OFFSET + 4] == t_report->peerBdAddr[1]) &&
                                    (data->dataBuf[BLE_ADDRESS_OFFSET + 3] == t_report->peerBdAddr[2]) &&
                                    (data->dataBuf[BLE_ADDRESS_OFFSET + 2] == t_report->peerBdAddr[3]) &&
                                    (data->dataBuf[BLE_ADDRESS_OFFSET + 1] == t_report->peerBdAddr[4]) &&
                                    (data->dataBuf[BLE_ADDRESS_OFFSET + 0] == t_report->peerBdAddr[5]))
                            {
                                isID_Match = true;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    dataType = DATA_FORMAT_V1_0_BLEBEACON;
                }
                Insert_New_Data(t_report, dataType, isID_Match);
            }
            
            /* If the condition is matched, blink LED and 
             * advertising data will be inserted into the ring buffer. */
            if(isID_Match)
            {
                /*Ready to blink LED*/
                ledBlinkCount = LED_FLICKER_TIME;
            }
            break;
        }
        
        default:
            break;
    }
}

/*******************************************************************************
* Function Name: isUARTTxFIFOEmpty
********************************************************************************
*
* Summary:
*  This function checks if UART Tx FIFO is empty
*
* Parameters:
*  NONE
*
* Return:
*  bool
*
* Theory:
*  NONE
*
* Side Effects:
*  NONE
*
* Note:
*
*******************************************************************************/
bool Is_UART_TxFIFO_Empty()
{
    /* Check if uart Tx buffer has already been empty*/
    return (0 == (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID));
}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  System entrance point. This calls the initializing function and
*  continuously process BLE events.
*
* Parameters:
*  NONE
*
* Return:
*  int - this is main loop and never returns
*
* Theory:
*  NONE
*
* Side Effects:
*  NONE
*
* Note:
*
*******************************************************************************/
int main()
{
    /* Enable global interrupt mask */
    CyGlobalIntEnable;

    /* This function will initialize the LED module */
    LED_Init();

    /* This function will initialize the ring buffer */
    Init_Ring_Buffer();

    /* Initialize SysTick, set reload time, register callback function to handle time up event*/
    CySysTickStart();
    CySysTickSetReload(SYSTICK_100MS);
    CySysTickSetCallback(0, SysTickISRCallback);

    /* Enable UART */
    UART_Start();

    /* Start BLE component and register the eventHandler function. This 
     * function exposes the events from BLE component for application use */
    CyBle_Start((CYBLE_CALLBACK_T)eventHandler);

    /* Start scanning */
    CyBle_GapcStartScan(CYBLE_SCANNING_FAST);

    for(;;)
    {
        /* UART FIFO hardware buffer is empty */
        if (Is_UART_TxFIFO_Empty())
        {
            /* Get bluetooth data from ring buffer and send to the PC by UART*/
            Handle_Bluetooth_Data();
        }

        /*Process event callback to handle BLE events. The events generated and 
        * used for this application are inside the 'eventHandler' routine*/
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
