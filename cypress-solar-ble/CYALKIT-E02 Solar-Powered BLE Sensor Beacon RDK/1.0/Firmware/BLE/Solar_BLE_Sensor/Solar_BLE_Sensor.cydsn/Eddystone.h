/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : Eddystone.h
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

#ifndef __EDDYSTONE_H_
#define __EDDYSTONE_H_

#include <project.h>

/******************************************************************************
* Macros and Constants
******************************************************************************/
#define ES_SVC_SOLI_LENGTH              0x03    /* Length of Service Solicitation */
#define ES_SVC_SOLICITATION             0x03    /* Type of Service Solicitation  */
#define ES_SVC_DATA                     0x16    /* Type of Service data          */
#define ES_SVC_LSB                      0xAA    /* LSB of Eddystone service UUID */
#define ES_SVC_MSB                      0xFE    /* MSB of Eddystone service UUID */
    
/* Frame Type */
#define ES_FRAME_TYPE_UID               0x00
#define ES_FRAME_TYPE_URL               0x10
#define ES_FRAME_TYPE_TLM               0x20
#define ES_FRAME_TYPE_INDEX_INFRAME     4
#define ES_FRAME_TYPE_INDEX_INARRAY     (ES_ADV_FLAGS_LENGTH + ES_ADV_SVC_UUID_LENGTH + ES_FRAME_TYPE_INDEX_INFRAME)
    
/* Return Codes */
#define RETURN_CODES_SUCCESS                        CYBLE_GATT_ERR_NONE
#define RETURN_CODES_WRITE_NOT_PERMITTED            CYBLE_GATT_ERR_WRITE_NOT_PERMITTED
#define RETURN_CODES_INSUFFICIENT_AUTHORIZATION     CYBLE_GATT_ERR_INSUFFICIENT_AUTHORIZATION
#define RETURN_CODES_INVALID_ATTRIBUTE_LEN          CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN
    
#define ES_LOCK_LENGTH                      16

/* TX Power Mode */
#define TX_POWER_MODE_HIGH                  3
#define TX_POWER_MODE_MEDIUM                2
#define TX_POWER_MODE_LOW                   1
#define TX_POWER_MODE_LOWEST                0
    
#define ES_TX_POWER_MODE_COUNT              4
#define ES_ADV_TXPL_INDEX_INFRAME           5
#define ES_ADV_TXPL_INDEX_INARRAY           (ES_ADV_FLAGS_LENGTH + ES_ADV_SVC_UUID_LENGTH + ES_ADV_TXPL_INDEX_INFRAME)
    
/*
 * Signal attenuation from transmitted power to received power at 0 meters, in dBm.
 * It's used for calculating the default Advertised TX Power Levels.
 * The way to determine the value is to measure the average RSSI from 1 meter away, 
 * add 41dBm to that, and then subtract the sum from the current TX Power Level. 
 * 41dBm is the signal loss that occurs over 1 meter.
 */
#define SIGNAL_ATTENUATION                  23      /* 23 dB */
    
#define ES_URL_FLAGS_DEFAULT                0x00
#define ES_BEACON_PERIOD_DEFAULT            100
#define ES_TXP_MODE_DEFAULT                 TX_POWER_MODE_HIGH
    
/* Eddystone advertisement, Flags, AD DATA 1 */
#define ES_ADV_FLAGS_LENGTH                 3
/* Eddystone advertisement, Flags, AD DATA 2 */
#define ES_ADV_SVC_UUID_LENGTH              4
#define ES_ADV_SVC_UUID_INDEX_LEN           3
#define ES_ADV_SVC_UUID_INDEX_TYPE          4
#define ES_ADV_SVC_UUID_INDEX_LSB           5
#define ES_ADV_SVC_UUID_INDEX_MSB           6
/* Eddystone UID, AD DATA 3 */
#define ES_UID_FRAME_LENGTH                 0x17
#define ES_UID_FRAME_ARRAY_LENGTH           (ES_UID_FRAME_LENGTH + 1)
#define ES_NID_INDEX_INFRAME                6
#define ES_BID_INDEX_INFRAME                (ES_NID_INDEX_INFRAME + NID_BYTES)
/* Eddystone URL, AD DATA 3 */
#define EDDYSTONE_URI_DATA_MAX_LEN          18
#define ES_URL_FRAME_ARRAY_MAXLEN           24
#define ES_URI_DATA_INDEX_INFRAME           6
/* Eddystone TLM, AD DATA 3 */
#define ES_TLM_FRAME_LENGTH                 0x11
#define ES_TLM_FRAME_ARRAY_LENGTH           (ES_TLM_FRAME_LENGTH + 1)
#define ES_TLM_INDEX_TEMP_MSB               15
#define ES_TLM_INDEX_TEMP_LSB               16
#define ES_TLM_NOTEMP_MSB                   0x80
#define ES_TLM_NOTEMP_LSB                   0x00
#define ES_TLM_INDEX_ADV_CNT                17
#define ES_TLM_INDEX_SEC_CNT                21
#define ES_TLM_VBATT_DEFAULT                0       /* Battery voltage function is not supported. */

#define ES_FRAME_OFFSET                     (ES_ADV_FLAGS_LENGTH + ES_ADV_SVC_UUID_LENGTH)
#define ATT_VALUES_LEN_INDEX_URIDATA        13      /* Index of URI_DATA in array cyBle_attValuesLen[] */
#define WRITE_ATTRIB_OFFSET_0               0       /* offset for CyBle_GattsWriteAttributeValue()     */

/* BLE adv. data offsets in cyBle_discoveryData.advData, Eddystone format */
#define ADV_TXPOWERLEVEL_INDEX              5
    
#define POWER_LOSS_IN_ANTENNA               4       /* the loss of 4dBm in the beacon antenna */
    
#define URL_PREFIX_MAX                      0x03    /* for the expansion https:// */
    
/******************************************************************************
* Global Variable Declaration
******************************************************************************/
extern uint8_t gatt_disconnect_flag;
    
/******************************************************************************
* External Function Prototypes
******************************************************************************/
void EddystoneURL_Config_Handler(uint32_t event, void * eventParam);
void EddystoneURL_Config_Init(void);
void EddystoneURL_Config_UpdateAllAttrib(void);

#endif /* __EDDYSTONE_H_ */

/* [] END OF FILE */
