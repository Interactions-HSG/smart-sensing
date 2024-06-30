/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : Eddystone.c
* Version           : CYALKIT-E02 Sample Firmware, Version 1.2.00
* Device Used       : CYBLE-022001-00
* Software Used     : PSoC Creator 3.3 CP3
* Compiler          : ARM GCC 4.9.3
* Related Hardware  : Solar BLE Sensor
*
*******************************************************************************
* Copyright (2015-2016), Cypress Semiconductor Corporation. All Rights Reserved.
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

#include "Eddystone.h"
#include "ConfigRW.h"
#include "UartCmd.h"


/******************************************************************************
* Global Variable Declarations
******************************************************************************/
uint8_t gatt_disconnect_flag = false;  /* It's set if a GATT client disconnects from the beacon. */

/******************************************************************************
* Function prototypes
******************************************************************************/
static void EddystoneURL_Config_Lock(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_Unlock(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_URI_Data(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_URI_Flags(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_URI_EDADPWR(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_URI_EDTXMODE(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_URI_BeaconPeriod(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);
static void EddystoneURL_Config_URI_Reset(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam);


/*******************************************************************************
* Function Name: SendErrorCode
********************************************************************************
* Summary:
*   Send Error code to GATT Client.
*
* Parameters:
*   Handle     - Connection handle to identify the peer GATT entity
*   attrHandle - Handle of characteristic
*   errorCode  - Return Error Code
*
* Return:
*  void
*
*******************************************************************************/
void SendErrorCode(CYBLE_CONN_HANDLE_T Handle, 
                   CYBLE_GATT_DB_ATTR_HANDLE_T  attrHandle, 
                   CYBLE_GATT_ERR_CODE_T errorCode)
{
    CYBLE_GATTS_ERR_PARAM_T err_param;
                    
    err_param.opcode = CYBLE_GATT_WRITE_REQ;
    err_param.attrHandle = attrHandle;
    err_param.errorCode = errorCode;

    /* Send Error Response */
    CyBle_GattsErrorRsp(Handle, &err_param);
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_Handler
********************************************************************************
* Summary:
*   This is an event callback function to receive GATT events for Eddystone-URL 
*   Configuration Service.
*
* Parameters:
*   event        - Event from the CYBLE component.
*   *eventParams - A structure instance for corresponding event type.
*
* Return:
*   void
*
*******************************************************************************/
void EddystoneURL_Config_Handler(uint32_t event, void * eventParam)
{
    /* This event is received when Central device sends a Write command on an Attribute. */
    if(event == CYBLE_EVT_GATTS_WRITE_REQ)
    {
        CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
        
        wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;
        
        switch(wrReqParam->handleValPair.attrHandle)
        {
            /* The request to locks the beacon and set the single-use lock-code. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_CHAR_HANDLE:
                EddystoneURL_Config_Lock(wrReqParam);
                break;
                
            /* The request to unlocks the beacon. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_UNLOCK_CHAR_HANDLE:
                EddystoneURL_Config_Unlock(wrReqParam);
                break;
                
            /* The request to write the URI. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE:
                EddystoneURL_Config_URI_Data(wrReqParam);
                break;
                
            /* The request to write the flags. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_HANDLE:
                EddystoneURL_Config_URI_Flags(wrReqParam);
                break;
                
            /* The request to write the Advertised Power Levels array. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE:
                EddystoneURL_Config_URI_EDADPWR(wrReqParam);
                break;
                
            /* The request to write the TX Power Mode. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE:
                EddystoneURL_Config_URI_EDTXMODE(wrReqParam);
                break;
                
            /* The request to write the Beacon Period. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE:
                EddystoneURL_Config_URI_BeaconPeriod(wrReqParam);
                break;
                
            /* The request to reset to default values. */
            case CYBLE_EDDYSTONEURL_CONFIGURATION_RESET_CHAR_HANDLE:
                EddystoneURL_Config_URI_Reset(wrReqParam);
                break;
            
            /* Writing others is not permitted. */
            default:
                SendErrorCode(wrReqParam->connHandle,
                              wrReqParam->handleValPair.attrHandle,
                              RETURN_CODES_WRITE_NOT_PERMITTED);
                break;
        }
    }
    /* event received when connection is established */
    else if(event == CYBLE_EVT_GAP_DEVICE_CONNECTED)
    {
        /* If user plans to do tests with eddystone-url-config-validator, make sure the state is unlocked. */
        if((pBeaconConfig->mode == ADV_MODE_EDDYSTONETEST) && pBeaconConfig->LockState)
        {
            uint8_t NewLockState = false;
            CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
            
            /* store the new lock state in SFlash */
            ConfigRW_UpdateConfig(CONFIG_TYPE_LOCKSTATE, &NewLockState);
            
            /* update Lock State characteristic */
            UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE;
            UpdateHandle.value.val  = &(pBeaconConfig->LockState);
            UpdateHandle.value.len  = sizeof(pBeaconConfig->LockState);
            CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                           WRITE_ATTRIB_OFFSET_0, 
                                           NULL, 
                                           CYBLE_GATT_DB_LOCALLY_INITIATED);
        }
    }
    /* GATT is disconnected. */
    else if(event == CYBLE_EVT_GATT_DISCONNECT_IND)
    {
        gatt_disconnect_flag = true;
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_Init
********************************************************************************
* Summary:
*   Initialize and start the Eddystone-URL Configuration Service.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void EddystoneURL_Config_Init(void)
{
    CyBle_Start((CYBLE_CALLBACK_T)EddystoneURL_Config_Handler);
    CyBle_ProcessEvents();
    
    EddystoneURL_Config_UpdateAllAttrib();
    
    cyBle_discoveryData.advData[ADV_TXPOWERLEVEL_INDEX] -= POWER_LOSS_IN_ANTENNA;
    
    CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_UpdateAllAttrib
********************************************************************************
* Summary:
*   Update all the values with data stored in User SFlash.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void EddystoneURL_Config_UpdateAllAttrib(void)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
    
    /* update Lock State */
    UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE;
    UpdateHandle.value.val  = &(pBeaconConfig->LockState);
    UpdateHandle.value.len  = sizeof(pBeaconConfig->LockState);
    CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                   WRITE_ATTRIB_OFFSET_0, 
                                   NULL, 
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    /* update URI Data */
    cyBle_attValuesLen[ATT_VALUES_LEN_INDEX_URIDATA].actualLength = pBeaconConfig->URI_Length;
    UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE;
    UpdateHandle.value.val  = pBeaconConfig->URI_Data;
    UpdateHandle.value.len  = pBeaconConfig->URI_Length;
    CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                   WRITE_ATTRIB_OFFSET_0, 
                                   NULL, 
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    /* update URI Flags */
    UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_HANDLE;
    UpdateHandle.value.val  = &(pBeaconConfig->URL_Flags);
    UpdateHandle.value.len  = sizeof(pBeaconConfig->URL_Flags);
    CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                   WRITE_ATTRIB_OFFSET_0, 
                                   NULL, 
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    /* update TX Power Levels */
    UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE;
    UpdateHandle.value.val  = (uint8_t *)(pBeaconConfig->Adv_TXPowerLevels);
    UpdateHandle.value.len  = ES_TX_POWER_MODE_COUNT;
    CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                   WRITE_ATTRIB_OFFSET_0, 
                                   NULL, 
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    /* update TX Power Mode */
    UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE;
    UpdateHandle.value.val  = &(pBeaconConfig->TXPowerMode);
    UpdateHandle.value.len  = sizeof(pBeaconConfig->TXPowerMode);
    CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                   WRITE_ATTRIB_OFFSET_0, 
                                   NULL, 
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    /* update Beacon Period */
    UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE;
    UpdateHandle.value.val  = (uint8_t *)(&(pBeaconConfig->URLPeriod));
    UpdateHandle.value.len  = sizeof(pBeaconConfig->URLPeriod);
    CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                   WRITE_ATTRIB_OFFSET_0, 
                                   NULL, 
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    return;
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_Lock
********************************************************************************
* Summary:
*   Function for locking the beacon and setting the single-use lock-code..
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_Lock(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* If the length is not right. */
    if(wrReqParam->handleValPair.value.len != ES_LOCK_LENGTH)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* The length is right, and the beacon is unlocked. */
    else
    {
        uint8_t NewLockState = true;
        CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
        
        /* store the new lock state and the new lock code in SFlash */
        ConfigRW_UpdateConfig(CONFIG_TYPE_LOCK, wrReqParam->handleValPair.value.val);
        ConfigRW_UpdateConfig(CONFIG_TYPE_LOCKSTATE, &NewLockState);
        
        /* update Lock State characteristic */
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE;
        UpdateHandle.value.val  = &(pBeaconConfig->LockState);
        UpdateHandle.value.len  = sizeof(pBeaconConfig->LockState);
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_Unlock
********************************************************************************
* Summary:
*   Function for unlocking the beacon.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_Unlock(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* If the length is not right. */
    if(wrReqParam->handleValPair.value.len != ES_LOCK_LENGTH)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_UNLOCK_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is unlocked. */
    else if(!pBeaconConfig->LockState)
    {
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
    /* The beacon is locked, the length is right, and the key is correct. */
    else if(memcmp(pBeaconConfig->lock, wrReqParam->handleValPair.value.val, ES_LOCK_LENGTH) == 0)
    {
        uint8_t NewLockState = false;
        CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
        
        /* store the new lock state in SFlash */
        ConfigRW_UpdateConfig(CONFIG_TYPE_LOCKSTATE, &NewLockState);
        
        /* update Lock State characteristic */
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE;
        UpdateHandle.value.val  = &(pBeaconConfig->LockState);
        UpdateHandle.value.len  = sizeof(pBeaconConfig->LockState);
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
    /* The beacon is locked, the length is right, but the key is invalid. */
    else
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_UNLOCK_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_URI_Data
********************************************************************************
* Summary:
*   Function for updating the uri data.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_URI_Data(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* The length is not correct. */
    if((wrReqParam->handleValPair.value.len == 0)                     || \
       (wrReqParam->handleValPair.value.len > EDDYSTONE_URI_DATA_MAX_LEN))
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* The beacon is unlocked, and the length is right. */
    else
    {
        int32_t i;
        bool isParaValid = true;
        
        /* Check whether the prefix is correct. */
        if(wrReqParam->handleValPair.value.val[0] > URL_PREFIX_MAX)
        {
            isParaValid = false;
        }
        /* Check the content after the prefix. */
        else
        {
            for(i = 1; i < wrReqParam->handleValPair.value.len; i++)
            {
                /* Range 00 - 13 is valid. */
                if (wrReqParam->handleValPair.value.val[i] < KINDS_OF_URL_SUFFIX)
                {
                    continue;
                }
                /* The octets 00-20 and 7F-FF hexadecimal are not used. */
                else if((wrReqParam->handleValPair.value.val[i] <= CHAR_LOWER_BOUND) || \
                        (wrReqParam->handleValPair.value.val[i] >= CHAR_UPPER_BOUND)    )
                {
                    isParaValid = false;
                    break;
                }
            }
        }
        
        /* The parameter is valid. */
        if(isParaValid)
        {
            /* store URI length and URI data in SFlash */
            ConfigRW_UpdateConfig(CONFIG_TYPE_URILENGTH, &(wrReqParam->handleValPair.value.len));
            ConfigRW_UpdateConfig(CONFIG_TYPE_URIDATA, wrReqParam->handleValPair.value.val);
            
            /* update URI DATA characteristic */
            cyBle_attValuesLen[ATT_VALUES_LEN_INDEX_URIDATA].actualLength = wrReqParam->handleValPair.value.len;
            CyBle_GattsWriteAttributeValue(&(wrReqParam->handleValPair), 
                                           WRITE_ATTRIB_OFFSET_0, 
                                           NULL, 
                                           CYBLE_GATT_DB_LOCALLY_INITIATED);
            
            CyBle_GattsWriteRsp(wrReqParam->connHandle);
        }
        /* The parameter is not valid. Deny the request. */
        else
        {
            SendErrorCode(wrReqParam->connHandle,
                          CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE, 
                          RETURN_CODES_WRITE_NOT_PERMITTED);
        }
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_URI_Flags
********************************************************************************
* Summary:
*   Function for updating the uri flags.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_URI_Flags(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* If the length is not right */
    if(wrReqParam->handleValPair.value.len != sizeof(pBeaconConfig->URL_Flags))
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* length is correct but the value is not 0 */
    else if(*(wrReqParam->handleValPair.value.val) != ES_URL_FLAGS_DEFAULT )
    {
        /* For now URI Flags can only be 0 */
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_HANDLE, 
                      RETURN_CODES_WRITE_NOT_PERMITTED);
    }
    /* length is correct and the value is 0 */
    else
    {
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_URI_EDADPWR
********************************************************************************
* Summary:
*   Function for updating the Advertised TX Power Levels array.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_URI_EDADPWR(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* length is not correct */
    if(wrReqParam->handleValPair.value.len != ES_TX_POWER_MODE_COUNT)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE,
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* The length is right, and the beacon is unlocked. */
    else
    {
        int32_t i;
        bool isParaValid = true;
        
        for(i = 0; i < ES_TX_POWER_MODE_COUNT; i++)
        {
            /* the value ranges from -100 dBm to +20 dBm to a resolution of 1 dBm. */
            if(((int8_t)wrReqParam->handleValPair.value.val[i] > ADPWR_UPPER_BOUND) ||
               ((int8_t)wrReqParam->handleValPair.value.val[i] < ADPWR_LOWER_BOUND)  )
            {
                isParaValid = false;
                break;
            }
        }
        
        /* The parameter is valid. */
        if(isParaValid)
        {
            /* store Advertised TX Power Levels in SFlash */
            ConfigRW_UpdateConfig(CONFIG_TYPE_EDADPWR, wrReqParam->handleValPair.value.val);
            /* update Advertised TX Power Levels characteristic */
            CyBle_GattsWriteAttributeValue(&(wrReqParam->handleValPair), 
                                           WRITE_ATTRIB_OFFSET_0, 
                                           NULL, 
                                           CYBLE_GATT_DB_LOCALLY_INITIATED);
            CyBle_GattsWriteRsp(wrReqParam->connHandle);
        }
        /* The parameter is not valid. Deny the request. */
        else
        {
            SendErrorCode(wrReqParam->connHandle,
                          CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE, 
                          RETURN_CODES_WRITE_NOT_PERMITTED);
        }
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_URI_EDTXMODE
********************************************************************************
* Summary:
*   Function for updating the TX Power Mode.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_URI_EDTXMODE(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* If length is not correct */
    if(wrReqParam->handleValPair.value.len != sizeof(pBeaconConfig->TXPowerMode))
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* If the value is out of range. */
    else if(*(wrReqParam->handleValPair.value.val) > TX_POWER_MODE_HIGH)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE, 
                      RETURN_CODES_WRITE_NOT_PERMITTED);
    }
    /* Both the length and the value are correct. */
    else
    {
        /* store TX Power Mode in SFlash */
        ConfigRW_UpdateConfig(CONFIG_TYPE_TXPMODE, wrReqParam->handleValPair.value.val);
        /* update TX Power Mode characteristic */
        CyBle_GattsWriteAttributeValue(&(wrReqParam->handleValPair), 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_URI_BeaconPeriod
********************************************************************************
* Summary:
*   Function for updating the period in milliseconds that a Eddystone-URL packet
*   is transmitted.
*   A value of zero disables Eddystone-URL transmissions.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_URI_BeaconPeriod(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* If length is not correct. */
    if(wrReqParam->handleValPair.value.len != sizeof(pBeaconConfig->URLPeriod))
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* The length is right, and the beacon is unlocked. */
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T PeriodHandle;
        uint16_t NewPeriod;
        
        /* Little Endian */
        NewPeriod = (wrReqParam->handleValPair.value.val[0]) |
                    (wrReqParam->handleValPair.value.val[1] << 8);
        /* If the value is less than the minimum. 
         * 0 is allowed.                          */
        if((NewPeriod > 0)           && \
           (NewPeriod < ITRVL_MIN_TIME) )
        {
            NewPeriod = ITRVL_MIN_TIME;
        }
        /* If the value is larger than the maximum. */
        else if(NewPeriod > ITRVL_MAX_TIME)
        {
            NewPeriod = ITRVL_MAX_TIME;
        }
        
        /* store Beacon Period in SFlash */
        ConfigRW_UpdateConfig(CONFIG_TYPE_URLPERIOD, &NewPeriod);
        
        PeriodHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE;
        PeriodHandle.value.val = (uint8_t *)(&NewPeriod);
        PeriodHandle.value.len = sizeof(pBeaconConfig->URLPeriod);
        
        /* update Beacon Period characteristic */
        CyBle_GattsWriteAttributeValue(&PeriodHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
}

/*******************************************************************************
* Function Name: EddystoneURL_Config_URI_Reset
********************************************************************************
* Summary:
*   Function for setting all characteristics to their initial values.
*
* Parameters:
*   wrReqParam : the request parameter received from Client
*
* Return:
*   None.
*
*******************************************************************************/
static void EddystoneURL_Config_URI_Reset(CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam)
{
    /* If length is not correct. */
    if(wrReqParam->handleValPair.value.len != sizeof(uint8_t))
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_RESET_CHAR_HANDLE, 
                      RETURN_CODES_INVALID_ATTRIBUTE_LEN);
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        SendErrorCode(wrReqParam->connHandle,
                      CYBLE_EDDYSTONEURL_CONFIGURATION_RESET_CHAR_HANDLE, 
                      RETURN_CODES_INSUFFICIENT_AUTHORIZATION);
    }
    /* The length is right, and the beacon is unlocked. */
    else
    {
        /* If the value is not 0, reset the configurations. */
        if(*(wrReqParam->handleValPair.value.val) != 0)
        {
            ConfigRW_WriteReset();
            EddystoneURL_Config_UpdateAllAttrib();
        }
        CyBle_GattsWriteRsp(wrReqParam->connHandle);
    }
}

/* [] END OF FILE */
