/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : UartCmd.h
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

#ifndef _UART_CMD_H
#define _UART_CMD_H

#include <project.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/******************************************************************************
* User Configure Area
******************************************************************************/
#define UART_NAME                           UART                            /* The name of uart component in TopDesign.cysch. */
#define UART_BAUDRATE                       115200                          /* The baudrate of uart. */
#define UART_BYTE_TX_TIME_MS                (10*1000/UART_BAUDRATE + 1)     /* Uart transfer time of one byte. */

/* string type Firmware version. */
#define GetFirmwareInfo(v)              sprintf(v,"-> CYALKIT-E02 Sample Firmware, Version %d.%d.%02d\r\n",FIRMWARE_MAJOR_VERSION,FIRMWARE_MINOR1_VERSION,FIRMWARE_MINOR2_VERSION)

/******************************************************************************
* Macros
******************************************************************************/
/* Function prototype to start the UART. */
#define UART_START_(NAME)                   NAME##_Start()
#define UART_START(NAME)                    UART_START_(NAME)
#define SIMPLE_UART_START()                 UART_START(UART_NAME)

/* Function prototype to disable the UART component. */
#define UART_STOP_(NAME)                    NAME##_Stop()
#define UART_STOP(NAME)                     UART_STOP_(NAME)
#define SIMPLE_UART_STOP()                  UART_STOP(UART_NAME)

/* Function prototype to places a byte of data in the transmit buffer to be sent. */
#define UART_PUT_CHAR_(NAME,C)              NAME##_UartPutChar(C)
#define UART_PUT_CHAR(NAME,C)               UART_PUT_CHAR_(NAME,C)
#define SIMPLE_UART_PUT_CHAR(C)             UART_PUT_CHAR(UART_NAME,C)

/* Function prototype to places a NULL terminated string in the transmit buffer to be sent. */
#define UART_PUTCONST_(NAME, X)             NAME##_UartPutString(X)
#define UART_PUTCONST(NAME, X)              UART_PUTCONST_(NAME, X)
#define SIMPLE_UART_PUTCONST(X)             UART_PUTCONST(UART_NAME, X)

/* Function prototype to return the number of received data elements in the receive buffer. */
#define UART_GET_RX_BUFFSIZE_(NAME)         NAME##_SpiUartGetRxBufferSize()
#define UART_GET_RX_BUFFSIZE(NAME)          UART_GET_RX_BUFFSIZE_(NAME)
#define SIMPLE_UART_GET_RX_BUFFSIZE()       UART_GET_RX_BUFFSIZE(UART_NAME)

/* Function prototype to return the number of elements currently in the transmit buffer. */
#define UART_GET_TX_BUFFSIZE_(NAME)         NAME##_SpiUartGetTxBufferSize()
#define UART_GET_TX_BUFFSIZE(NAME)          UART_GET_TX_BUFFSIZE_(NAME)
#define SIMPLE_UART_GET_TX_BUFFSIZE()       UART_GET_TX_BUFFSIZE(UART_NAME)

/* Function prototype to retrieve next data element from the receive buffer. */
#define UART_GET_BYTE_(NAME)                NAME##_UartGetByte()
#define UART_GET_BYTE(NAME)                 UART_GET_BYTE_(NAME)
#define SIMPLE_UART_GET_BYTE()              UART_GET_BYTE(UART_NAME)

#define UART_MAX_DATA_LEN                   (54)    /* 53 bytes to store user input along with 1 byte to store '\0'. */

#define CMD_CHARS_COUNT_MAX                 (sizeof(cmd_EDTXMODE))

#define SIZE_OF_FIRMWARE_INFO_BUF           128     /* Size of firmware format buffer. */

/* Size of ' '. */
#define SIZE_OF_PARAM_SPACE                 (1)
#define SIZE_OF_PARAM_A_NUMBER              (1)     /* A figure occupys a byte. */
#define SIZE_OF_NUL                         (1)     /* The character '\0' occupys a byte. */
#define NIBBLE_BITS                         (4)     /* Used to left shift 4 bits. */
/* Size of byte value format to "%02X". */
#define SIZE_OF_BYTE_TO_02X                 2
    
/* UUID related macro. */
#define UUID_TMP_BUFFER_40_BYTE             40

/* Size of UUID param with 4 '-'. */
#define SIZE_OF_UUID_PARAM                  (16*2+4)

/* Offset of each destination byte position 
   when source byte value format to "%02X". */
#define FORMAT_H_OFFSET                     0
#define FORMAT_L_OFFSET                     1

/* Hyphen position in string type uuid. */
#define UUID_STR_HYPHEN_POS_1               (8)
#define UUID_STR_HYPHEN_POS_2               (13)
#define UUID_STR_HYPHEN_POS_3               (18)
#define UUID_STR_HYPHEN_POS_4               (23)

/* Hyphen position in ASCII type uuid. */
#define UUID_HYPHEN_POS_1                   (4)
#define UUID_HYPHEN_POS_2                   (6)
#define UUID_HYPHEN_POS_3                   (8)
#define UUID_HYPHEN_POS_4                   (10)
    
/* Size of EDLOCK param with 4 '-'. */
/* xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx */
#define SIZE_OF_LOCK_CODE_PARAM             (16*2+4)
    
/* Hyphen position in string type lock code. */
#define LOCKCODE_STR_HYPHEN_POS_1           (8)
#define LOCKCODE_STR_HYPHEN_POS_2           (13)
#define LOCKCODE_STR_HYPHEN_POS_3           (18)
#define LOCKCODE_STR_HYPHEN_POS_4           (23)

/* Hyphen position in ASCII type lock code. */
#define LOCKCODE_HYPHEN_POS_1               (4)
#define LOCKCODE_HYPHEN_POS_2               (6)
#define LOCKCODE_HYPHEN_POS_3               (8)
#define LOCKCODE_HYPHEN_POS_4               (10)
    
/* Size of MAJOR param and related macro definition. */
#define SIZE_OF_MAJOR_PARAM                 (4)
#define MAJOR_HIGH_BYTE_OFFSET              0
#define MAJOR_LOW_BYTE_OFFSET               2
#define BIT_PER_BYTE                        8

/* Size of MINOR param and related macro definition. */
#define SIZE_OF_MINOR_PARAM                 (4)
#define MINOR_HIGH_BYTE_OFFSET              0
#define MINOR_LOW_BYTE_OFFSET               2

/* RSSI related macro. */
#define RSSI_OFFSET_OF_MINUS                0
#define RSSI_MAX_VALID_VALUE                (127l)
#define RSSI_MIN_VALID_VALUE                (-128l)
#define RSSI_INVALID_PARAM_LEN              1
#define RSSI_MINUS_INVALID_PARAM_LEN        2
#define RSSI_LOW_8BIT_KEEP_FLAG             0x000000FF

/* ITRVL valid time size and value. */
#define ITRVL_MIN_TIME_SIZE                 3           /* length of 100 is 3 */
#define ITRVL_MAX_TIME_SIZE                 5           /* length of 10240 is 5 */
#define ITRVL_MAX_TIME                      10240       /* unit: millisecond */
#define ITRVL_MIN_TIME                      100         /* unit: millisecond */

/*TXPWR related macro. */
#define TXPWR_MINUS_VALID_LEN_1             2
#define TXPWR_MINUS_VALID_LEN_2             3
#define TXPWR_MINUS_VALUE_OFFSET            1
#define TXPWR_ACCEPTED_VALUE_NEG_18         (-18l)
#define TXPWR_ACCEPTED_VALUE_NEG_12         (-12l)
#define TXPWR_ACCEPTED_VALUE_NEG_6          (-6l)
#define TXPWR_ACCEPTED_VALUE_NEG_3          (-3l)
#define TXPWR_ACCEPTED_VALUE_NEG_2          (-2l)
#define TXPWR_ACCEPTED_VALUE_NEG_1          (-1l)
#define TXPWR_ACCEPTED_VALUE_0              (0l)
#define TXPWR_ACCEPTED_VALUE_3              (3l)
#define TXPWR_ACCEPTED_COUNT                8           /* 8 kinds of accepted value in command txpwr and edtxpwr. */
#define TXPWR_PLUS_VALID_VALUE_LEN          1

/* Size of COID param and related macro definition. */
#define SIZE_OF_COID_PARAM                  4
#define COID_HIGH_BYTE_OFFSET               0
#define COID_LOW_BYTE_OFFSET                2

/* Temporary buffer size. */
#define TMP_BUFFER_4_BYTE                   4

/* Byte value switch between upper 4 bits and lower 4 bits. */
#define BYTE_H4_L4_BIT_SWITCH(v)            (uint8_t)((((char *)v)[0] << 4) | ((char *)v)[1])
    
#define KINDS_OF_URL_PREFIX                 4           /* kinds of url scheme prefix. */
#define KINDS_OF_URL_SUFFIX                 14          /* kinds of url scheme suffix. */

#define SLASH_COUNT_3                       3
    
#define CHAR_LOWER_BOUND                    0x20        /* The lower bound of the character in uri. */
#define CHAR_UPPER_BOUND                    0x7F        /* The upper bound of the character in uri. */

#define ADPWR_LOWER_BOUND                   (-100)      /* The lower bound of the Advertised TX Power Levels for Eddystone. */
#define ADPWR_UPPER_BOUND                   20          /* The upper bound of the Advertised TX Power Levels for Eddystone. */

#define ADPWR_INPUT_LONGEST_NUM             4           /* The longest length of num of the Advertised TX Power Levels for Eddystone, like "-xxx". */

/* UART command */
/* The string that will be sent over the UART when the application starts. */
#define START_STRING                        "\r\nStart ...\r\n"
/* The string that will be sent over the UART when user input is invalid. */
#define STRING_CMD_ERROR                    "Command format error!!\r\n"
#define STRING_CMD_UPDATE_REJECTED          "Insufficient Authorization!!\r\n"

#define RESULT_OK                           (0u)        /* UART input is OK. */
#define RESULT_ERROR                        (1u)        /* UART input is incorrect. */
#define RESULT_REJECTED                     (2u)        /* Because of the lock state, the updating operation is rejected. */
#define RESULT_EXIT                         (0x1000)    /* UART input is "exit\r\n". */
    
#define RESULT_NEWLINE                      (0xFF)      /* Get '\n' and append it to the end of str. */
#define RESULT_NO_NEWLINE                   (-1)        /* Get some characters except '\n' and append them to the end of str. */
#define RESULT_NO_INPUT                     (-2)        /* No uart input. */
#define RESULT_INVALID_PARAMETER            (-3)        /* The parameter is invalid. */
    
/* Return error value in function atox(). */
#define ATOX_PARAM_ERROR                    0xFF

/* Return error value in function twoaton(). */
#define TWOATON_PARAM_ERROR                 0xFFFF


/******************************************************************************
* External Function Prototypes
******************************************************************************/
int32_t UartCmd_Init(uint8_t *buffer);
int32_t UartCmd_ProcessInput(void);

#endif /* _UART_CMD_H */

/* [] END OF FILE */
