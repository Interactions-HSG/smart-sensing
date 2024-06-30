/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : ConfigRW.h
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

#ifndef __CONFIGRW_H_
#define __CONFIGRW_H_

#include <project.h>
#include <stdbool.h>
#include "WriteUserSFlash.h"
#include "Eddystone.h"

/******************************************************************************
* Macros and Constants
******************************************************************************/
#define SFLASH_DATA_FLAG_VALID          ((int32_t)0x000055AA)       /* This Flag is used to check the validity of user settings saved in SFlash.
                                                                     * It should not be 0xFFFFFFFF or 0. */
#define SFLASH_DATA_INIT_VALUE          (0xFF)                      /* Default value of sFlash data. After erasing, all bits in SFlash is 1. */
    
#define ADV_MODE_BLEBEACON              0
#define ADV_MODE_EDDYSTONE              1
#define ADV_MODE_EDDYSTONETEST          2
    
/* Error return values */
#define E_CFGRW_OK                      (0u)                        /* No error. */
#define E_CFGRW_PAR                     (-1)                        /* Incorrect parameter. */
#define E_CFGRW_WR_FAIL                 (-2)                        /* Failed to write User SFlash. */
    
#define UUID_BYTES                      16                          /* Size of UUID */
#define NID_BYTES                       10                          /* Size of NID */
#define BID_BYTES                       6                           /* Size of BID */
#define USED_SPACE_IN_SFLASH            112                         /* Number of bytes that members in BeaconConfig take, excluding reserved[]. */

#define FIRMWARE_MAJOR_VERSION          1                           /* Firmware major version */
#define FIRMWARE_MINOR1_VERSION         2                           /* Firmware minor1 version*/
#define FIRMWARE_MINOR2_VERSION         0                           /* Firmware minor2 version*/

#define FW_VERSION                      ((FIRMWARE_MAJOR_VERSION << 16) + FIRMWARE_MINOR1_VERSION)
    
/******************************************************************************
* User Configuration Macros
******************************************************************************/
#define BLE_SFLASH_ROW_NUM              (1u)                        /* User SFlash row number where user settings are saved. */

#define APP_BEACON_UUID                 0x00, 0x05, 0x00, 0x01, \
                                        0x00, 0x00, 0x10, 0x00, \
                                        0x80, 0x00, 0x00, 0x80, \
                                        0x5F, 0x9B, 0x01, 0x31      /* Default UUID for Beacon. */

#define APP_MAJOR_VALUE                 (1u)                        /* Default major value for BLEBeacon mode. */
#define APP_MINOR_VALUE                 (1u)                        /* Default minor value for BLEBeacon mode. */
#define APP_RSSI_VALUE                  (-61)                       /* Default rssi value for BLEBeacon mode. */

#define APP_ITRVL_VALUE                 (1500u)                     /* Default interval of advertising for BLEBeacon mode. */
#define APP_TXPWR_VALUE                 (3u)                        /* Default tx power for BLEBeacon mode. */
#define APP_COID_VALUE                  (0x004c)                    /* Default company id for BLEBeacon mode. */
#define APP_SENSOR_VALUE                (true)                      /* Default sensor mode for Beacon. */
    
#define ADV_MODE_DEFAULT                ADV_MODE_BLEBEACON
    
#define ES_FRAME_UID_DEFAULT            false
#define ES_FRAME_URL_DEFAULT            true
#define ES_FRAME_TLM_DEFAULT            false
    
#define ES_LOCK_STATE_DEFAULT           false                       /* unlocked */
#define ES_LOCK_DEFAULT                 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00      /* Default lock for Eddystone. */
    
#define URI_LEGTH_DEFAULT               9                           /* size of URI_DATA_DEFAULT */
#define URI_DATA_DEFAULT                0x00, 'c', 'y', 'p', 'r', 'e', 's', 's', 0x00   /* http://www.cypress.com/ */
    
#define APP_EDDYSTONE_NID               0xCB, 0x6F, 0x15, 0xCE, \
                                        0xC0, 0x2A, 0x41, 0xF7, \
                                        0x6A, 0xB1,                 /* Default NID for Eddystone. */

#define APP_EDDYSTONE_BID               0x00, 0x00, 0x00, 0x01, \
                                        0x00, 0x01                  /* Default BID for Eddystone. */
    
/* Default Eddystone TX Power Levels */
#define DEFAULT_TX_POWER_HIGH           (3)     /*   3dBm */
#define DEFAULT_TX_POWER_MEDIUM         (0)     /*   0dBm */
#define DEFAULT_TX_POWER_LOW            (-12)   /* -12dBm */
#define DEFAULT_TX_POWER_LOWEST         (-18)   /* -18dBm */
    
/******************************************************************************
* Enumerated Data Definition
******************************************************************************/
typedef enum
{
    CONFIG_TYPE_UUID,
    CONFIG_TYPE_MAJOR,
    CONFIG_TYPE_MINOR,
    CONFIG_TYPE_ITRVL,
    CONFIG_TYPE_COMID,
    
    CONFIG_TYPE_TXPWR,
    CONFIG_TYPE_RSSI,
    
    CONFIG_TYPE_SENSOR,
    CONFIG_TYPE_MODE,
    
    CONFIG_TYPE_NID,
    CONFIG_TYPE_BID,
    
    CONFIG_TYPE_EDTXPWR,
    CONFIG_TYPE_FRAME,
    
    CONFIG_TYPE_LOCKSTATE,
    CONFIG_TYPE_LOCK,
    
    CONFIG_TYPE_URILENGTH,
    CONFIG_TYPE_URIDATA,
    CONFIG_TYPE_URLFLAGS,
    CONFIG_TYPE_EDADPWR,
    CONFIG_TYPE_TXPMODE,
    CONFIG_TYPE_URLPERIOD,
    
    CONFIG_TYPE_ALL
}ConfigType;                          /* Stands for the type of what user want to update.
                                       * Used by ConfigRW_UpdateConfig(). */
    
/******************************************************************************
* Data Struct Definition
******************************************************************************/
typedef struct
{
    uint8_t UIDisEnabled:1;
    uint8_t URLisEnabled:1;
    uint8_t TLMisEnabled:1;
    uint8_t reserved:5;
}EDframeFlag;

typedef struct
{
    /* This flag is needed in case power failure after uuid is erased and before DataFlag_End is erases. 
     * WriteUserSFlashRow function writes this struct by uint32_t, so int32_t is chosen as the variable type. */
    int32_t     DataFlag_Start;
    
    uint8_t     uuid[UUID_BYTES];           /* for BLEBeacon */
    uint16_t    major;                      /* for BLEBeacon */
    uint16_t    minor;                      /* for BLEBeacon */
    uint16_t    itrvl;                      /* for BLEBeacon. If URLPeriod is 0, Eddystone use itrvl instead. */
    uint16_t    comID;                      /* for BLEBeacon */
    
    int8_t      txpwr;                      /* for BLEBeacon */
    int8_t      rssi;                       /* for BLEBeacon */
    
    int8_t      sensor;                     /* for both modes */
    int8_t      mode;                       /* Advertisement is BLEBeacon mode or Eddystone mode */
    
    uint8_t     nid[NID_BYTES];                             /* for Eddystone */
    uint8_t     bid[BID_BYTES];                             /* for Eddystone */
    
    int8_t      TXPowerLevels[ES_TX_POWER_MODE_COUNT];      /* for Eddystone */
    
    EDframeFlag frame;                                      /* for Eddystone */
    
    uint8_t     LockState;                                  /* for Eddystone */
    uint8_t     lock[ES_LOCK_LENGTH];                       /* for Eddystone */
    
    uint8_t     URI_Length;                                 /* for Eddystone-URL */
    uint8_t     URI_Data[EDDYSTONE_URI_DATA_MAX_LEN];       /* for Eddystone-URL */
    uint8_t     URL_Flags;                                  /* for Eddystone-URL, reserved flags for future use, must be set to 0. */
    
    int8_t      Adv_TXPowerLevels[ES_TX_POWER_MODE_COUNT];  /* for Eddystone */
    uint8_t     TXPowerMode;                                /* for Eddystone */

    uint16_t    URLPeriod;                                  /* for Eddystone. If it's 0, disable URL frame and use itrvl instead. */
    
    /* Member reserved[] is not used yet. The purpose of reserved[] is 
     * to reserve an area where user can store newly added settings.
     * When adding new member, length of reserved[] must be modified to make sure
     * offset of DataFlag_End and sizeof(BeaconConfig) are not changed. */
    int32_t     reserved[(USER_SFLASH_ROW_SIZE - USED_SPACE_IN_SFLASH)/sizeof(int32_t)];
    
    uint32_t    version_minor2;
    uint32_t    version_major_minor1;
    /* This flag is needed in case power failure after DataFlag_Start is written and before version_major_minor1 is written. */
    int32_t     DataFlag_End;
}BeaconConfig;                        /* Read and write user settings by struct. 
                                       * This is the the storage format of the latest version. 
                                       * sizeof(BeaconConfig) must be equal to USER_SFLASH_ROW_SIZE. */

typedef struct
{
    uint8_t uuid[UUID_BYTES];
    uint16_t major;
    uint16_t minor;
    uint16_t itrvl;
    uint16_t comID;
    
    int8_t txpwr;
    int8_t rssi;
    int8_t sensor;
    
    int32_t SFlashDataFlag;
}BeaconConfig_1V0;                      /* Storage format of version 1.0 */

/******************************************************************************
* Global Variable Declaration
******************************************************************************/
extern const BeaconConfig DefaultBeaconConfig;
#define pBeaconConfig       ((BeaconConfig *)(USER_SFLASH_BASE_ADDRESS + BLE_SFLASH_ROW_NUM * USER_SFLASH_ROW_SIZE))
#define pBeaconConfig_1V0   ((BeaconConfig_1V0 *)(USER_SFLASH_BASE_ADDRESS + BLE_SFLASH_ROW_NUM * USER_SFLASH_ROW_SIZE))

/******************************************************************************
* External Function Prototypes
******************************************************************************/
int32_t ConfigRW_UpdateConfig(ConfigType ct, const void *ptr);
int32_t ConfigRW_CheckVersion(void);
int32_t ConfigRW_WriteDefault(void);
int32_t ConfigRW_WriteReset(void);


#if ((BLE_SFLASH_ROW_NUM < 1) || (BLE_SFLASH_ROW_NUM > 3))
    #error "BLE_SFLASH_ROW_NUM value is incorrect."
#endif

#endif /* __CONFIGRW_H_ */

/* [] END OF FILE */
