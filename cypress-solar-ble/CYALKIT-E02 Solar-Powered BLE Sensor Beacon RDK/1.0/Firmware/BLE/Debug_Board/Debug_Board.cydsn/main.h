/******************************************************************************
* Project Name      : Debug_Board
* File Name         : main.h
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

#ifndef _MAIN_H_
#define _MAIN_H_
#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*****************************************************************************
* Macros Definition
*****************************************************************************/
/*compile option aligned 1 byte*/
#define PACKED __attribute__( ( packed, aligned(1) ) )

/* buffer,clock,led flicker time configure */
#define MAX_BUFFER_NUMBER           10
#define SYSTICK_100MS               (CySysTickGetReload() * 100)
#define LED_FLICKER_TIME            2                /* 2 * 100 ms */
#define FLICKER_PERIOD              2                /* flicker 200ms */
#define LIGHT_INTERVAL              1                /* light   100ms */
#define UART_SEND_BUFFER_SIZE       128

/* LED On Intensity */
#define DEVICE_LED_ON_INTENSITY     0

/* UART filter command*/
#define CTRLFLAG_FILTER_ON          ('1')
    
#define MAGIC_NUM_LENGTH            2
#define TYPE_LENGTH                 1
#define BLE_ADDRESS_LENGTH          6
#define RSSI_LENGTH                 1
#define HEADER_LENGTH               (MAGIC_NUM_LENGTH+TYPE_LENGTH+BLE_ADDRESS_LENGTH+RSSI_LENGTH)
#define BLE_ADV_MAX_LENGTH          31

#define TYPE_OFFSET                 (MAGIC_NUM_LENGTH)
#define BLE_ADDRESS_OFFSET          (MAGIC_NUM_LENGTH+TYPE_LENGTH)
#define RSSI_OFFSET                 (MAGIC_NUM_LENGTH+TYPE_LENGTH+BLE_ADDRESS_LENGTH)
#define BLE_ADV_OFFSET              (HEADER_LENGTH)

/* Expected UUID */
#define EXPECTED_UUID_BYTE1             (0x00)
#define EXPECTED_UUID_BYTE2             (0x05)
#define EXPECTED_UUID_BYTE3             (0x00)
#define EXPECTED_UUID_BYTE4             (0x01)

/* Expected NID */
#define EXPECTED_NID_BYTE1             (0xCB)
#define EXPECTED_NID_BYTE2             (0x6F)
#define EXPECTED_NID_BYTE3             (0x15)
#define EXPECTED_NID_BYTE4             (0xCE)

    
/* Eddystone-UID Bluetooth data offset and value */
/* Service Solicitation data, Length */
#define ES_SVC_SOLI_LEN_OFFSET          3
#define ES_SVC_SOLI_LEN_VALUE           (0x03)
/* Service Solicitation data, Type */
#define ES_SVC_SOLI_TYPE_OFFSET         4
#define ES_SVC_SOLI_TYPE_VALUE          (0x03)
/* 16bit Eddystone UUID - MSB */
#define ES_SVC_SOLI_SVC_MSB_OFFSET      5
#define ES_SVC_SOLI_SVC_MSB_VALUE       (0xAA)
/* 16bit Eddystone UUID - LSB */
#define ES_SVC_SOLI_SVC_LSB_OFFSET      6
#define ES_SVC_SOLI_SVC_LSB_VALUE       (0xFE)

/* Service Solicitation data, Type */
#define ES_SVC_SOLI_TYPE2_OFFSET        8
#define ES_SVC_SOLI_TYPE2_VALUE         (0x16)
/* 16bit Eddystone UUID - MSB */
#define ES_SVC_SOLI_SVC2_MSB_OFFSET     9
/* 16bit Eddystone UUID - LSB */
#define ES_SVC_SOLI_SVC2_LSB_OFFSET     10

/* Eddystone service */
#define ES_SVC_SOLI_FRAME_TYPE_OFFSET   11
#define ES_SVC_SOLI_FRAME_TYPE_UID      (0x00)
#define ES_SVC_SOLI_FRAME_TYPE_URL      (0x10)
#define ES_SVC_SOLI_FRAME_TYPE_TLM      (0x20)

/* NID in Eddystone-UID frame */
#define ES_SVC_SOLI_NID_OFFSET          13
    
    
/* BLEBeacon Bluetooth data offset and value */
#define BLEBEACON_DATA2_LENGTH_OFFSET     3
#define BLEBEACON_DATA2_LENGTH_VALUE1     (0x1A)     /* Standard data length */
#define BLEBEACON_DATA2_LENGTH_VALUE2     (0x1B)     /* One additional byte is included */
    
#define BLEBEACON_DATA2_TYPE_OFFSET       4
#define BLEBEACON_DATA2_TYPE_VALUE        (0xFF)
    
#define BLEBEACON_DEVICE_TYPE_OFFSET      7
#define BLEBEACON_DEVICE_TYPE_VALUE       (0x02)
    
#define BLEBEACON_DATA2_UUID_OFFSET       9


/* Data Format */
#define DATA_FORMAT_V1_0_BLEBEACON     (0x0100)
#define DATA_FORMAT_V2_0_RAW           (0x0200)
#define DATA_FORMAT_V2_0_EDDYSTONE     (0x0201)

/* Data Frag */
#define DATA_FRAG_CY_DEVICE            (0x0001)

/*****************************************************************************
* Enumerated Data Definition
*****************************************************************************/

/*ring buffer data status*/
typedef enum
{
    /* Buffer is not in used */
    CYBLE_BUFFER_IDLE     = 0,

    /* Insert new data into buffer */
    CYBLE_BUFFER_INSERT   = 1,

    /* Data is expired */
    CYBLE_BUFFER_EXPIRED  = 20  /* 20 * 100ms */
}BUFFER_STATUS_T;

/*****************************************************************************
* Data Struct Definition
*****************************************************************************/

/*  data struct for store report data*/
typedef struct
{
    uint8_t dataBuf[HEADER_LENGTH + BLE_ADV_MAX_LENGTH];
    uint16_t dataType;
    uint16_t flag;
}REPORT_DATA_T;

/* report data buffer struct for store report data */
typedef struct tagRptDataBuffer
{
    /* store received data from bluetooth */
    REPORT_DATA_T rptData;

    /* report data length */
    uint32_t dataLen;

    /* report data status */
    uint32_t dataStatus;

    /* pointer to next data in ring buffer */
    struct tagRptDataBuffer * pNext;
}REPORT_DATA_BUFFER_T;

#endif /*_MAIN_H_*/

/* [] END OF FILE */
