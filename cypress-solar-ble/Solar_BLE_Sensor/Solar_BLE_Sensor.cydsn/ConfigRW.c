/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : ConfigRW.c
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

#include "ConfigRW.h"

/******************************************************************************
* Global Variable Declaration
******************************************************************************/
const BeaconConfig DefaultBeaconConfig =
{
    SFLASH_DATA_FLAG_VALID,             /* int32_t DataFlag_Start */
    
    { APP_BEACON_UUID },                /* uint8_t uuid[] */
    APP_MAJOR_VALUE,                    /* uint16_t major */
    APP_MINOR_VALUE,                    /* uint16_t minor */
    APP_ITRVL_VALUE,                    /* uint16_t itrvl */
    APP_COID_VALUE,                     /* uint16_t comID */
    
    APP_TXPWR_VALUE,                    /* int8_t txpwr   */
    APP_RSSI_VALUE,                     /* int8_t rssi    */
    
    APP_SENSOR_VALUE,                   /* int8_t sensor  */
    ADV_MODE_DEFAULT,                   /* int8_t mode    */
    
    { APP_EDDYSTONE_NID },              /* uint8_t nid[]  */
    { APP_EDDYSTONE_BID },              /* uint8_t bid[]  */
    
    {                                   /* int8_t TXPowerLevels[] */
        DEFAULT_TX_POWER_LOWEST,
        DEFAULT_TX_POWER_LOW,
        DEFAULT_TX_POWER_MEDIUM,
        DEFAULT_TX_POWER_HIGH
    },
    
    {                                   /* EDframeFlag frame  */
        ES_FRAME_UID_DEFAULT,
        ES_FRAME_URL_DEFAULT,
        ES_FRAME_TLM_DEFAULT,
        0                               /* Reserved bits */
    },
    
    ES_LOCK_STATE_DEFAULT,              /* uint8_t LockState */
    { ES_LOCK_DEFAULT },                /* uint8_t lock      */
    
    URI_LEGTH_DEFAULT,                  /* uint8_t URI_Length */
    { URI_DATA_DEFAULT },               /* uint8_t URI_Data[] */
    
    ES_URL_FLAGS_DEFAULT,               /* uint8_t URL_Flags */
    
    {                                   /* int8_t Adv_TXPowerLevels[] */
        DEFAULT_TX_POWER_LOWEST - SIGNAL_ATTENUATION, 
        DEFAULT_TX_POWER_LOW    - SIGNAL_ATTENUATION, 
        DEFAULT_TX_POWER_MEDIUM - SIGNAL_ATTENUATION, 
        DEFAULT_TX_POWER_HIGH   - SIGNAL_ATTENUATION
    }, 
    ES_TXP_MODE_DEFAULT,                /* uint8_t TXPowerMode */
    
    ES_BEACON_PERIOD_DEFAULT,           /* uint16_t URLPeriod  */
    
    {},                                 /* int32_t reserved[]  */
    
    FIRMWARE_MINOR2_VERSION,            /* uint32_t version_minor2         */
    FW_VERSION,                         /* uint32_t version_major_minor1   */
    SFLASH_DATA_FLAG_VALID              /* int32_t DataFlag_End            */
};                                      /* default configuration */

/******************************************************************************
* Function Name: ConfigRW_UpdateConfig
*******************************************************************************
*
* Summary:
*   This function is used to update the configuration data stored in User SFlash, 
*   it could update part of the configuration or the whole configuration data.
*   The data format in User SFlash cannot be older than the current version.
*   Update URI_Length before updating URI_Data!
*
* Parameters:
*   ct - Any member of enum ConfigType. It's the type of what user want to update.
*   ptr - The pointer to the new configuration data.
*
* Return:
*   int32_t - Return value indicates if the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   E_CFGRW_OK                          No error.
*   E_CFGRW_PAR                         Incorrect parameter.
*   E_CFGRW_WR_FAIL                     Failed to write User SFlash.
******************************************************************************/
int32_t ConfigRW_UpdateConfig(ConfigType ct, const void *ptr)
{
    BeaconConfig *pBCtemp;
    int32_t temp[USER_SFLASH_ROW_SIZE / sizeof(int32_t)];
    
    /* Check if parameter is invalid*/
    if (NULL == ptr)
    {
        return E_CFGRW_PAR;
    }
    /* If all the settings need to be updated. */
    if(ct == CONFIG_TYPE_ALL)
    {
        pBCtemp = (BeaconConfig *)ptr;
    }
    /* If just one setting needs to be updated. */
    else
    {
        pBCtemp = (BeaconConfig *)temp;
        
        memcpy(pBCtemp, pBeaconConfig, sizeof(BeaconConfig));
        
        switch(ct)
        {
            case CONFIG_TYPE_UUID:
                /* User just want to update UUID. */
                memcpy(pBCtemp->uuid, ptr, sizeof(pBCtemp->uuid));
                break;
                
            case CONFIG_TYPE_MAJOR:
                /* User just want to update MAJOR. */
                pBCtemp->major = *(uint16_t *)ptr;
                break;
                
            case CONFIG_TYPE_MINOR:
                /* User just want to update MINOR. */
                pBCtemp->minor = *(uint16_t *)ptr;
                break;
                
            case CONFIG_TYPE_ITRVL:
                /* User just want to update advertising interval in BLEBeacon mode. */
                pBCtemp->itrvl = *(uint16_t *)ptr;
                break;
                
            case CONFIG_TYPE_COMID:
                /* User just want to update company id. */
                pBCtemp->comID = *(uint16_t *)ptr;
                break;
                
            case CONFIG_TYPE_TXPWR:
                /* User just want to update tx power in BLEBeacon mode. */
                pBCtemp->txpwr = *(int8_t *)ptr;
                break;
                
            case CONFIG_TYPE_RSSI:
                /* User just want to update rssi value in BLEBeacon mode. */
                pBCtemp->rssi = *(int8_t *)ptr;
                break;
                
            case CONFIG_TYPE_SENSOR:
                /* User just want to update sensor mode. */
                pBCtemp->sensor = *(int8_t *)ptr;
                break;
                
            case CONFIG_TYPE_MODE:
                /* User just want to update the advertisement mode. */
                pBCtemp->mode = *(int8_t *)ptr;
                break;
                
            case CONFIG_TYPE_NID:
                /* User just want to update NID. */
                memcpy(pBCtemp->nid, ptr, sizeof(pBCtemp->nid));
                break;
                
            case CONFIG_TYPE_BID:
                /* User just want to update BID. */
                memcpy(pBCtemp->bid, ptr, sizeof(pBCtemp->bid));
                break;
                
            case CONFIG_TYPE_EDTXPWR:
                /* User just want to update edtxpwr. */
                memcpy(pBCtemp->TXPowerLevels, ptr, ES_TX_POWER_MODE_COUNT);
                break;
                
            case CONFIG_TYPE_FRAME:
                /* User just want to update Eddystone frames. */
                memcpy(&(pBCtemp->frame), ptr, sizeof(EDframeFlag));
                break;
                
            case CONFIG_TYPE_LOCKSTATE:
                /* User just want to update the lock state. */
                pBCtemp->LockState = *(uint8_t *)ptr;
                break;
                
            case CONFIG_TYPE_LOCK:
                /* User just want to update Eddystone-URL lock-code. */
                memcpy(pBCtemp->lock, ptr, sizeof(pBCtemp->lock));
                break;
                
            case CONFIG_TYPE_URILENGTH:
                /* User just want to update URI_Length. */
                pBCtemp->URI_Length = *(uint8_t *)ptr;
                break;
                
            case CONFIG_TYPE_URIDATA:
                /* User just want to update URI_Data. */
                memcpy(pBCtemp->URI_Data, ptr, pBCtemp->URI_Length);
                break;
                
            case CONFIG_TYPE_URLFLAGS:
                /* User just want to update URL_Flags. */
                pBCtemp->URL_Flags = *(uint8_t *)ptr;
                break;
                
            case CONFIG_TYPE_EDADPWR:
                /* User just want to update Advertised TX Power Levels in Eddystone mode. */
                memcpy(pBCtemp->Adv_TXPowerLevels, ptr, ES_TX_POWER_MODE_COUNT);
                break;
                
            case CONFIG_TYPE_TXPMODE:
                /* User just want to update TXPowerMode in Eddystone mode. */
                pBCtemp->TXPowerMode = *(uint8_t *)ptr;
                break;
                
            case CONFIG_TYPE_URLPERIOD:
                /* User just want to update Beacon Period in Eddystone mode. */
                pBCtemp->URLPeriod = *(uint16_t *)ptr;
                break;
                
            default:
                return E_CFGRW_PAR;
        }
    }
    
    /* Check writing result. If it's successful, return ok. */
    if(WriteUserSFlashRow(BLE_SFLASH_ROW_NUM, (uint32_t *)pBCtemp) == USER_SFLASH_WRITE_SUCCESSFUL)
    {
        return E_CFGRW_OK;
    }
    /* If it fails, inform the user. */
    else
    {
        return E_CFGRW_WR_FAIL;
    }
}

/******************************************************************************
* Function Name: ConfigRW_CheckVersion
*******************************************************************************
*
* Summary:
*   This function checks data stored in User SFlash. 
*   If there is no data, it writes the default configuration into User SFlash.
*   If the version of the stored data is old, update the data with the latest 
*   format, the data value will not change.
*
* Parameters:
*   None
*
* Return:
*   int32_t - Return value indicates if the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   E_CFGRW_OK                          No error.
*   E_CFGRW_WR_FAIL                     Failed to write User SFlash.
******************************************************************************/
int32_t ConfigRW_CheckVersion(void)
{
    /* Try resolving the configuration in User SFlash with the latest format. */
    if( (pBeaconConfig->DataFlag_End == SFLASH_DATA_FLAG_VALID) && \
        (pBeaconConfig->DataFlag_Start == SFLASH_DATA_FLAG_VALID)  )
    {
        /* If the format is older than the current version. */
        if(pBeaconConfig->version_major_minor1 < FW_VERSION)
        {
            /* There's configuration data of version 1.1 format in User SFlash. */
            int32_t temp[USER_SFLASH_ROW_SIZE / sizeof(int32_t)];
            BeaconConfig *pBCtemp = (BeaconConfig *)temp;

            memcpy(pBCtemp, &DefaultBeaconConfig, sizeof(BeaconConfig));
            memset(pBCtemp->reserved, SFLASH_DATA_INIT_VALUE, sizeof(pBCtemp->reserved));
        
            memcpy(pBCtemp->uuid, pBeaconConfig->uuid, sizeof(pBeaconConfig->uuid));
            pBCtemp->major  = pBeaconConfig->major;
            pBCtemp->minor  = pBeaconConfig->minor;
            pBCtemp->itrvl  = pBeaconConfig->itrvl;
            pBCtemp->comID  = pBeaconConfig->comID;
            
            pBCtemp->txpwr  = pBeaconConfig->txpwr;
            pBCtemp->rssi   = pBeaconConfig->rssi;
            pBCtemp->sensor = pBeaconConfig->sensor;
        
            return ConfigRW_UpdateConfig(CONFIG_TYPE_ALL, temp);
        }
        /* The format is newer than or same as the current version. */
        else
        {
            return E_CFGRW_OK;
        }
    }
    /* Try resolving the configuration in User SFlash with the version 1.0 format. */
    else if(pBeaconConfig_1V0->SFlashDataFlag == SFLASH_DATA_FLAG_VALID)
    {
        int32_t temp[USER_SFLASH_ROW_SIZE / sizeof(int32_t)];
        BeaconConfig *pBCtemp = (BeaconConfig *)temp;
        
        memcpy(pBCtemp, &DefaultBeaconConfig, sizeof(BeaconConfig));
        memset(pBCtemp->reserved, SFLASH_DATA_INIT_VALUE, sizeof(pBCtemp->reserved));
        
        memcpy(pBCtemp->uuid, pBeaconConfig_1V0->uuid, sizeof(pBeaconConfig_1V0->uuid));
        
        pBCtemp->major  = pBeaconConfig_1V0->major;
        pBCtemp->minor  = pBeaconConfig_1V0->minor;
        pBCtemp->itrvl  = pBeaconConfig_1V0->itrvl;
        pBCtemp->comID  = pBeaconConfig_1V0->comID;
        
        pBCtemp->txpwr  = pBeaconConfig_1V0->txpwr;
        pBCtemp->rssi   = pBeaconConfig_1V0->rssi;
        pBCtemp->sensor = pBeaconConfig_1V0->sensor;
        
        return ConfigRW_UpdateConfig(CONFIG_TYPE_ALL, temp);
    }
    /* Data in User SFlash cannot be resolved, copy default settings to user SFlash. */
    else
    {
        return ConfigRW_WriteDefault();
    }
}

/******************************************************************************
* Function Name: ConfigRW_WriteDefault
*******************************************************************************
*
* Summary:
*   This function writes the default configuration into User SFlash.
*
* Parameters:
*   None
*
* Return:
*   int32_t - Return value indicates if the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   E_CFGRW_OK                          No error.
*   E_CFGRW_WR_FAIL                     Failed to write User SFlash.
******************************************************************************/
int32_t ConfigRW_WriteDefault(void)
{
    int32_t temp[USER_SFLASH_ROW_SIZE / sizeof(int32_t)];
    BeaconConfig *pBCtemp = (BeaconConfig *)temp;
    
    memcpy(pBCtemp, &DefaultBeaconConfig, sizeof(BeaconConfig));
    
    memset(pBCtemp->reserved, SFLASH_DATA_INIT_VALUE, sizeof(pBCtemp->reserved));
    
    return ConfigRW_UpdateConfig(CONFIG_TYPE_ALL, temp);
}

/******************************************************************************
* Function Name: ConfigRW_WriteReset
*******************************************************************************
*
* Summary:
*   This function writes the default configuration of the eddystone mode into 
*   User SFlash, excluding EDNID, EDBID and EDFRAME.
*
* Parameters:
*   None
*
* Return:
*   int32_t - Return value indicates if the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   E_CFGRW_OK                          No error.
*   E_CFGRW_WR_FAIL                     Failed to write User SFlash.
******************************************************************************/
int32_t ConfigRW_WriteReset(void)
{
    int32_t temp[USER_SFLASH_ROW_SIZE / sizeof(int32_t)];
    BeaconConfig *pBCtemp = (BeaconConfig *)temp;
    
    memcpy(pBCtemp, pBeaconConfig, sizeof(BeaconConfig));
    
    memcpy(pBCtemp->TXPowerLevels, DefaultBeaconConfig.TXPowerLevels, ES_TX_POWER_MODE_COUNT);
    
    /* Change parameters from LockState to URLPeriod to default. */
    memcpy(&pBCtemp->LockState, 
           &DefaultBeaconConfig.LockState, 
           ((void *)DefaultBeaconConfig.reserved - (void *)(&DefaultBeaconConfig.LockState)));
    
    return ConfigRW_UpdateConfig(CONFIG_TYPE_ALL, temp);
}

/* [] END OF FILE */
