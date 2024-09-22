/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : main.h
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

#if !defined (MAIN_H)
#define MAIN_H
    
#include <project.h>

/******************************************************************************
* Macros and Constants
******************************************************************************/
/* If enabled, temperature and humidity sensor can be enabled by set sensor 
 * setting to on.
 * If disabled, firmware won't support the sensor, sensor setting has no effect. */
#define I2C_SENSOR_ENABLE                       (1u)

/* When USB powered, USB_Detect_Read() returns 0x01. */
#define PIN_HIGH                                0x01
    
#define WCO_FREQUENCY                           32768       /* 32768Hz */

/* WDT counter settings */
#define COUNTER_ENABLE                          (1u)        /* 1 to enable WDT counter */
/* WDT counter for low power sleep mode */
#define SOURCE_COUNTER                          (0u)        /* use WDT counter 0 */
#define COUNT_PERIOD_WCO                        ((uint32_t)(WCO_FREQUENCY/2 - 1))     /* 500ms */
#define COUNT_PERIOD_ECO                        ((uint32_t)115)                       /* 3.5ms */
#define COUNT_PERIOD_1S                         ((uint32_t)(WCO_FREQUENCY - 1))       /* 1s */
#define COUNT_PERIOD_100MS                      ((uint32_t)(WCO_FREQUENCY/10 - 1))    /* 100ms */
#define WDT_INTERRUPT_SOURCE                    CY_SYS_WDT_COUNTER0_INT /* WDT counter 0 interrupt pending bit */
/* WDT counter for low power sleep mode and setting flag */
#define I2C_COUNTER                             (1u)        /* use WDT counter 1 */
#define I2C_COUNT_PERIOD_2S                     ((uint32_t)(WCO_FREQUENCY*2 - 1))         /* 2s */
#define I2C_COUNT_PERIOD_1S                     ((uint32_t)(WCO_FREQUENCY - 1))           /* 1s */
#define I2C_COUNT_PERIOD_5MS                    ((uint32_t)(WCO_FREQUENCY*5/1000 - 1))    /* 5ms */
#define I2C_COUNT_PERIOD_10MS                   ((uint32_t)(WCO_FREQUENCY*10/1000 - 1))   /* 10ms */
#define I2C_COUNT_PERIOD_20MS                   ((uint32_t)(WCO_FREQUENCY*20/1000 - 1))   /* 20ms */
#define I2C_COUNT_PERIOD_180MS                  ((uint32_t)(WCO_FREQUENCY*180/1000 - 1))  /* 180ms */
#define I2C_WDT_INTERRUPT_SOURCE                CY_SYS_WDT_COUNTER1_INT /* WDT counter 1 interrupt pending bit */
    
#define BEACON_PERIOD_SENS_ON_1FRAME            6100    /* If the beacon is in sensor on mode, and only one frame
                                                           is advertised, the period is about 6100 ms. */

#define CYBLE_ADV_NO_TIMEOUT                    0
#define ADV_DATA1_FLAGS_INDEX                   2
#define ADV_DATA1_FLAGS_NON_CONNECTABLE         0x04
    
/* In any AD DATA of the Bluetooth advertisement packet, 
 * the first byte stands for length excluding itself. */
#define INDEX_LEN_IN_BLE_AD_DATA                0
#define SIZE_LEN_IN_BLE_AD_DATA                 1
    
/* BLE adv. data offsets in cyBle_discoveryData.advData, BLEBeacon format */
#define BLEBEACON_ADV_LENGTH                    30
    
#define BLEBEACON_LENGTH2_OFFSET                3
#define BLEBEACON_LENGTH2_VALUE                 0x1A
    
#define BLEBEACON_AD_TYPE2_OFFSET               4
#define BLEBEACON_AD_TYPE2_VALUE                0xFF
    
#define BLEBEACON_DEVICE_TYPE_OFFSET            7
#define BLEBEACON_DEVICE_TYPE_VALUE             0x02
    
#define BLEBEACON_LENGTH3_OFFSET                8
#define BLEBEACON_LENGTH3_VALUE                 0x15
    
#define ADDR_COID_OFFSET                        5                                   /* COID offset */
#define SIZE_COID                               2                                   /* length of COID in bytes */

#define ADDR_UUID_OFFSET                        9                                   /* UUID offset */
#define SIZE_UUID                               16                                  /* length of UUID in bytes */

#define ADDR_MAJOR_OFFSET                       (ADDR_UUID_OFFSET + SIZE_UUID)      /* MAJOR offset: 25 */
#define SIZE_MAJOR                              2                                   /* length of MAJOR in bytes */

#define ADDR_MINOR_OFFSET                       (ADDR_MAJOR_OFFSET + SIZE_MAJOR)    /* MINOR offset: 27 */
#define SIZE_MINOR                              2                                   /* length of MINOR in bytes */
    
#define ADDR_HUM_OFFSET                         (ADDR_MAJOR_OFFSET + SIZE_MAJOR)    /* Humidity offset: 27 */
#define SIZE_HUM                                1                                   /* length of Humidity in bytes */

#define ADDR_TEM_OFFSET                         (ADDR_HUM_OFFSET + SIZE_HUM)        /* Temperature offset: 28 */
#define SIZE_TEM                                1                                   /* length of Temperature in bytes */

#define ADDR_RSSI_OFFSET                        (ADDR_TEM_OFFSET + SIZE_TEM)        /* RSSI offset: 29 */
#define SIZE_RSSI                               1                                   /* length of RSSI in bytes */

/* BLE adv. data offsets in cyBle_discoveryData.advData, Eddystone format */
#define ES_ADV_HUMI_OFFSET                      23        /* store humidity at BID[0]    */
#define ES_ADV_TEMP_OFFSET                      24        /* store temperature at BID[1] */
    
/*
 * Value assigned to advIntvMin or advIntvMax = (Time in millisecond)/0.625
 *                                            = (Time in millisecond)*8/5
 *                                            = ((Time in millisecond) << 3) / 5
 */
#define CALC_REG_INTERVAL(T_ms)                 ((T_ms << 3) / 5)
/*
 * Convert float*100 to float*256.
 * Temperature in Eddystone-TLM frame is expressed in a signed 8.8 fixed-point notation,
 * which means the resolution is 1/256.
 */
#define FLOAT100_TO_FLOAT256(F100)              ((F100)*256/100)
    
#define GET_BYTE1_BIG_ENDIAN(v32)               (((uint32_t)v32 >> 24) & 0xFF)
#define GET_BYTE2_BIG_ENDIAN(v32)               (((uint32_t)v32 >> 16) & 0xFF)
#define GET_BYTE3_BIG_ENDIAN(v32)               (((uint32_t)v32 >> 8) & 0xFF)
#define GET_BYTE4_BIG_ENDIAN(v32)               (((uint32_t)v32 >> 0) & 0xFF)
    
#define RATIO_0_1SEC_TO_MS                      100 /* The ratio of 0.1sec to ms is 100. */
    

/******************************************************************************
* Enumerated Data Definition
******************************************************************************/
/* Temperature and Humidity sensor state machine */
typedef enum
{
    /* Default state. I2C peripheral and sensor are not initialized. */
    I2C_START = 0x00,

    /* Get ready to read Humidity. */
    I2C_READ_HUMIDITY_SEND_CMD,

    /* Read Humidity. */
    I2C_READ_HUMIDITY_RECV_DATA,

    /* Get ready to read temperature. */
    I2C_READ_TEMP_SEND_CMD,

    /* Read temperature. */
    I2C_READ_TEMP_RECV_DATA,

    /* Start advertisement of BLEBeacon or Eddystone-UID. */
    I2C_START_ADV,
    
    /* Advertise Eddystone-URL frame. */
    I2C_ADV_URL,
    
    /* Advertise Eddystone-TLM frame. */
    I2C_ADV_TLM,
    
    /* Stop advertisement. */
    I2C_STOP_ADV,
    
    /* Wait until the solar cell gathers enough power. */
    I2C_WAIT
}I2C_SENSOR_APP_STATE;

/* Beacon state machine.
 * To lower power requirement, advertisement won't start immediately after the 
 * system is initialized when sensor setting is off. */
typedef enum
{
    /* Default state. Wait after the system is initialized. */
    BEACON_WAIT = 0x00,
    
    /* Start advertisement when the current state is BEACON_START. */
    BEACON_START,
    
    /* The system is advertising BLEBeacon data. Noting to do except sleep. */
    BEACON_RUN,
    
    /* Update advertisement data to Eddystone-UID frame. */
    BEACON_UID,
    
    /* Update advertisement data to Eddystone-URL frame. */
    BEACON_URL,
    
    /* Update advertisement data to Eddystone-TLM frame. */
    BEACON_TLM,
    
    /* If only TLM frame is enabled, ADV_CNT and SEC_CNT must be updated everytime. */
    BEACON_TLMCNT
}BEACON_APP_STATE;

#endif  /* MAIN_H */

/* [] END OF FILE */
