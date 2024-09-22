/******************************************************************************
* Project Name      : Solar BLE Sensor
* File Name         : UartCmd.c
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

#include "main.h"
#include "UartCmd.h"
#include "ConfigRW.h"
#include <stdint.h>

/******************************************************************************
* Private Function Prototypes
******************************************************************************/
static uint16_t cmd_sub_uuid(void);
static uint16_t cmd_sub_major(void);
static uint16_t cmd_sub_minor(void);
static uint16_t cmd_sub_rssi(void);
static uint16_t cmd_sub_itrvl(void);
static uint16_t cmd_sub_txpwr(void);
static uint16_t cmd_sub_coid(void);

static uint16_t cmd_sub_ednid(void);
static uint16_t cmd_sub_edbid(void);
static uint16_t cmd_sub_edtxpwr(void);
static uint16_t cmd_sub_edframe(void);
static uint16_t cmd_sub_lock(void);
static uint16_t cmd_sub_unlock(void);
static uint16_t cmd_sub_eduri(void);
static uint16_t cmd_sub_edadpwr(void);
static uint16_t cmd_sub_edtxmode(void); 
static uint16_t cmd_sub_editrvl(void);
static uint16_t cmd_sub_edreset(void);

static uint16_t cmd_sub_exsens(void);
static uint16_t cmd_sub_mode(void);
static uint16_t cmd_sub_version(void);
static uint16_t cmd_sub_init(void);

static uint16_t hextobin(uint8_t *dest, const uint8_t *src, uint8_t Nbytes);
static uint16_t twoaton(const uint8_t *p);
static uint8_t atox(uint8_t data);

/******************************************************************************
* Global Variable Declaration
******************************************************************************/
static const uint8_t cmd_UUID[]     = {'U', 'U', 'I', 'D'};
static const uint8_t cmd_MAJOR[]    = {'M', 'A', 'J', 'O', 'R'};
static const uint8_t cmd_MINOR[]    = {'M', 'I', 'N', 'O', 'R'};
static const uint8_t cmd_TXPWR[]    = {'T', 'X', 'P', 'W', 'R'};
static const uint8_t cmd_RSSI[]     = {'R', 'S', 'S', 'I'};
static const uint8_t cmd_ITRVL[]    = {'I', 'T', 'R', 'V', 'L'};
static const uint8_t cmd_COID[]     = {'C', 'O', 'I', 'D'};

static const uint8_t cmd_EDNID[]    = {'E', 'D', 'N', 'I', 'D'};
static const uint8_t cmd_EDBID[]    = {'E', 'D', 'B', 'I', 'D'};
static const uint8_t cmd_EDTXPWR[]  = {'E', 'D', 'T', 'X', 'P', 'W', 'R'};
static const uint8_t cmd_EDFRAME[]  = {'E', 'D', 'F', 'R', 'A', 'M', 'E'};
static const uint8_t cmd_EDSTATE[]  = {'E', 'D', 'S', 'T', 'A', 'T', 'E'};
static const uint8_t cmd_EDLOCK[]   = {'E', 'D', 'L', 'O', 'C', 'K'};
static const uint8_t cmd_EDUNLOCK[] = {'E', 'D', 'U', 'N', 'L', 'O', 'C', 'K'};
static const uint8_t cmd_EDURI[]    = {'E', 'D', 'U', 'R', 'I'};
static const uint8_t cmd_EDADPWR[]  = {'E', 'D', 'A', 'D', 'P', 'W', 'R'};
static const uint8_t cmd_EDTXMODE[] = {'E', 'D', 'T', 'X', 'M', 'O', 'D', 'E'};
static const uint8_t cmd_EDITRVL[]  = {'E', 'D', 'I', 'T', 'R', 'V', 'L'};
static const uint8_t cmd_EDRESET[]  = {'E', 'D', 'R', 'E', 'S', 'E', 'T'};

static const uint8_t cmd_EXSENS[]   = {'S', 'E', 'N', 'S', 'O', 'R'};
static const uint8_t cmd_MODE[]     = {'M', 'O', 'D', 'E'};
static const uint8_t cmd_INIT[]     = {'I', 'N', 'I', 'T'};
static const uint8_t cmd_EXIT[]     = {'E', 'X', 'I', 'T'};
static const uint8_t cmd_VER[]      = {'V', 'E', 'R'};
static const uint8_t cmd_HELP[]     = {'H', 'E', 'L', 'P'};

/* Pointer to the array we used to store the UART data. */
static uint8_t *data_array;

/******************************************************************************
* Function Name: UartCmd_ReadLineAppend
*******************************************************************************
*
* Summary:
*   Function for getting UART data. This function will receive character from 
*   the UART and append it to a null terminated string.
*
* Parameters:  
*   str - pointer to a null terminated string.
*  
* Return:
*   int32_t - Return value indicates what the function have done: 
*               get the end of the input,
*               get some char, but not get the end
*               get nothing,
*               find that the parameter is invalid. 
*
*   Following are the possible state codes.
*   
*   State codes                        Description
*   ------------                        -----------
*   RESULT_NEWLINE                      Get new line character and append it to the end of str.
*   RESULT_NO_NEWLINE                   Get some characters except '\n' and append them to the end of str.
*   RESULT_NO_INPUT                     No uart input.
*   RESULT_INVALID_PARAMETER            The parameter is invalid.
******************************************************************************/
int32_t UartCmd_ReadLineAppend(uint8_t *str)
{
    uint8_t index = 0;

    /* If no uart input */
    if(SIMPLE_UART_GET_RX_BUFFSIZE() == 0)
    {
        return RESULT_NO_INPUT;
    }
    
    index = strlen((const char *)str);
    /* If str is full of data */
    if(index >= (UART_MAX_DATA_LEN - 1))
    {
        SIMPLE_UART_PUTCONST("Reset buffer to 0 after handling it.\r\n");
        return RESULT_INVALID_PARAMETER;
    }
    
    /* If rx fifo is not empty, get all the data in rx fifo. */
    while(SIMPLE_UART_GET_RX_BUFFSIZE())
    {
        data_array[index] = SIMPLE_UART_GET_BYTE();
        index++;

        /* If user presses Enter, or user inputs too many characters, 
         * echo and return user input as a NULL terminated string. */
        if((data_array[index - 1] == '\n') || (index >= (UART_MAX_DATA_LEN - 1)))
        {
            data_array[index] = 0;
            
            SIMPLE_UART_PUTCONST((const char *)data_array);
            while(SIMPLE_UART_GET_TX_BUFFSIZE() > 0); /* wait until tx fifo is empty. */
            CyDelay(UART_BYTE_TX_TIME_MS);            /* wait until the last byte is transferred. */
            
            return RESULT_NEWLINE;
        }
    }
    
    return RESULT_NO_NEWLINE; /* line doesn't end */
}

/******************************************************************************
* Function Name: UartCmd_Init
*******************************************************************************
*
* Summary:
*   Initialize UART and data for UART command.
*
* Parameters:
*   buffer - memory space larger than UART_MAX_DATA_LEN to store UART input.
*
* Return:
*   int32_t - Return value indicates whether the function succeeded or failed.
* 
*   Following are the possible state codes.
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_INVALID_PARAMETER            buffer is NULL.
******************************************************************************/
int32_t UartCmd_Init(uint8_t *buffer)
{
    SIMPLE_UART_START();
    
    SIMPLE_UART_PUTCONST(START_STRING);
#if (!I2C_SENSOR_ENABLE)
    SIMPLE_UART_PUTCONST("I2C sensor is disabled. Ignore sensor setting.\r\n");
#endif
    if(buffer != NULL)
    {
        data_array = buffer;
        memset(data_array, 0, UART_MAX_DATA_LEN);
        return RESULT_OK;
    }
    else
    {
        return RESULT_INVALID_PARAMETER;
    }
}

/******************************************************************************
* Function Name: UartCmd_ProcessInput
*******************************************************************************
*
* Summary:
*   Function for processing UART commands.
*
* Parameters:  
*   None.
*  
* Return:
*   int32_t - Return value indicates if the format of the input is right, 
*   and if the input is just the command "exit" we expected.
*   Following are the possible state codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_EXIT                         The uart command is exit.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
int32_t UartCmd_ProcessInput(void)
{
    uint16_t result = RESULT_ERROR;
    uint8_t data_length;
    uint8_t cmd_tmp_upr[CMD_CHARS_COUNT_MAX + SIZE_OF_NUL];
    
    /* If user hasn't pressed Enter, we wait. */
    if(UartCmd_ReadLineAppend(data_array) != RESULT_NEWLINE)
    {
        return RESULT_OK;
    }
    
    data_length = strlen((char *)data_array);
    /* If data_array do not end with '\r\n', it's not the format we expect to receive. */
    if((data_array[data_length - 2] == '\r') && \
       (data_array[data_length - 1] == '\n')    )
    {
        /* Replace the end "\r\n\0" with "\0\0\0" */
        data_array[data_length - 2] = '\0';
        data_array[data_length - 1] = '\0';
        
        memcpy(cmd_tmp_upr, data_array, CMD_CHARS_COUNT_MAX);
        cmd_tmp_upr[CMD_CHARS_COUNT_MAX] = '\0';
        strupr((char *)cmd_tmp_upr);
        
        /* EDURI */
        if(memcmp(cmd_EDURI, cmd_tmp_upr, sizeof(cmd_EDURI)) == 0)
        {
            result = cmd_sub_eduri();
        }
        /* UUID */
        else if(memcmp(cmd_UUID, cmd_tmp_upr, sizeof(cmd_UUID)) == 0)
        {
            result = cmd_sub_uuid();
        }
        /* EDNID */
        else if(memcmp(cmd_EDNID, cmd_tmp_upr, sizeof(cmd_EDNID)) == 0)
        {
            result = cmd_sub_ednid();
        }
        /* EDBID */
        else if(memcmp(cmd_EDBID, cmd_tmp_upr, sizeof(cmd_EDBID)) == 0)
        {
            result = cmd_sub_edbid();
        }
        /* MAJOR */
        else if(memcmp(cmd_MAJOR, cmd_tmp_upr, sizeof(cmd_MAJOR)) == 0)
        {
            result = cmd_sub_major();
        }
        /* MINOR */
        else if(memcmp(cmd_MINOR, cmd_tmp_upr, sizeof(cmd_MINOR)) == 0)
        {
            result = cmd_sub_minor();
        }
        /* BLEBeacon advertise interval */
        else if(memcmp(cmd_ITRVL, cmd_tmp_upr, sizeof(cmd_ITRVL)) == 0)
        {
            result = cmd_sub_itrvl();
        }
        /* Eddystone advertise interval */
        else if(memcmp(cmd_EDITRVL, cmd_tmp_upr, sizeof(cmd_EDITRVL)) == 0)
        {
            result = cmd_sub_editrvl();
        }
        /* TX power */
        else if(memcmp(cmd_TXPWR, cmd_tmp_upr, sizeof(cmd_TXPWR)) == 0)
        {
            result = cmd_sub_txpwr();
        }
        /* Eddystone Radio TX Power Levels */
        else if(memcmp(cmd_EDTXPWR, cmd_tmp_upr, sizeof(cmd_EDTXPWR)) == 0)
        {
            result = cmd_sub_edtxpwr();
        }
        /* Eddystone Advertised TX Power Levels */
        else if(memcmp(cmd_EDADPWR, cmd_tmp_upr, sizeof(cmd_EDADPWR)) == 0)
        {
            result = cmd_sub_edadpwr();
        }
        /* Eddystone TX Power Mode */
        else if(memcmp(cmd_EDTXMODE, cmd_tmp_upr, sizeof(cmd_EDTXMODE)) == 0)
        {
            result = cmd_sub_edtxmode();
        }
        /* EDRESET */
        else if(memcmp(cmd_EDRESET, cmd_tmp_upr, sizeof(cmd_EDRESET)) == 0)
        {
            result = cmd_sub_edreset();
        }
        /* RSSI */
        else if(memcmp(cmd_RSSI, cmd_tmp_upr, sizeof(cmd_RSSI)) == 0)
        {
            result = cmd_sub_rssi();
        }
        /* Company ID */
        else if(memcmp(cmd_COID, cmd_tmp_upr, sizeof(cmd_COID)) == 0)
        {
            result = cmd_sub_coid();
        }
        /* HELP */
        else if(memcmp(cmd_HELP, cmd_tmp_upr, sizeof(cmd_HELP)) == 0)
        {
            /* If user just pressed Enter after "HELP". */
            if(data_array[sizeof(cmd_HELP)] == '\0')
            {
                /* BLEBeacon commands */
                SIMPLE_UART_PUTCONST("Get uuid, input:uuid+Enter\r\n"
                                     "Set uuid, input:uuid xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get major, input:major+Enter\r\n"
                                     "Set major, input:major xxxx+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get minor, input:minor+Enter\r\n"
                                     "Set minor, input:minor xxxx+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get txpwr, input:txpwr+Enter\r\n"
                                     "Set txpwr, input:txpwr *+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get rssi, input:rssi+Enter\r\n"
                                     "Set rssi, input:rssi *+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get itrvl, input:itrvl+Enter\r\n"
                                     "Set itrvl, input:itrvl *+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get coid, input:coid+Enter\r\n"
                                     "Set coid, input:coid xxxx+Enter\r\n");
                SIMPLE_UART_PUTCONST("\r\n");
                /* Eddystone commands */
                SIMPLE_UART_PUTCONST("Get nid, input: ednid+Enter\r\n"
                                     "Set nid, input: ednid x...x(20 char)+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get bid, input: edbid+Enter\r\n"
                                     "Set bid, input: edbid x...x(12 char)+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get edtxpwr, input:edtxpwr+Enter\r\n"
                                     "Set edtxpwr, input:edtxpwr * * * *+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get edframe, input:edframe+Enter\r\n"
                                     "Set edframe, input:edframe + one or some of uid/url/tlm+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get edstate, input: edstate+Enter\r\n");
                SIMPLE_UART_PUTCONST("Set edlock, input: edlock xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx+Enter\r\n");
                SIMPLE_UART_PUTCONST("Set edunlock, input: edunlock xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get eduri, input:eduri+Enter\r\n"
                                     "Set eduri, input:eduri (url or hex string)+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get edadpwr, input:edadpwr+Enter\r\n"
                                     "Set edadpwr, input:edadpwr * * * *+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get edtxmode, input:edtxmode+Enter\r\n"
                                     "Set edtxmode, input:edtxmode LOWEST/LOW/MEDIUM/HIGH+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get editrvl, input:editrvl+Enter\r\n"
                                     "Set editrvl, input:editrvl *+Enter\r\n");
                SIMPLE_UART_PUTCONST("edreset, input: edreset+Enter\r\n");
                SIMPLE_UART_PUTCONST("\r\n");
                /* Common commands */
                SIMPLE_UART_PUTCONST("Get sensor, input:sensor+Enter\r\n"
                                     "Set sensor, input:sensor on/off+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get mode,input: mode+Enter\r\n"
                                     "Set mode,input: mode BLEBeacon/Eddystone/EDTest+Enter\r\n");
                SIMPLE_UART_PUTCONST("Initialize, input: init+Enter\r\n");
                SIMPLE_UART_PUTCONST("Get ver, input: ver+Enter\r\n");
                SIMPLE_UART_PUTCONST("Exit, input: exit+Enter\r\n");
                
                result = RESULT_OK;
            }
            /* There's more data than "HELP". */
            else
            {
                /* result is still RESULT_ERROR. */
            }
        }
        /* Version */
        else if(memcmp(cmd_VER, cmd_tmp_upr, sizeof(cmd_VER)) == 0)
        {
            result = cmd_sub_version();
        }
        /* Exit */
        else if(memcmp(cmd_EXIT, cmd_tmp_upr, sizeof(cmd_EXIT)) == 0)
        {
            /* If user just presses Enter after "exit". */
            if(data_array[sizeof(cmd_EXIT)] == '\0')
            {
                result = RESULT_EXIT;
            }
            /* If user input more than "exit", command is invalid. */
            else
            {
                /* result is still RESULT_ERROR. */
            }
        }
        /* temperature and humidity sensor extension */
        else if(memcmp(cmd_EXSENS, cmd_tmp_upr, sizeof(cmd_EXSENS)) == 0)
        {
            result = cmd_sub_exsens();
        }
        /* mode of advertisement */
        else if(memcmp(cmd_MODE, cmd_tmp_upr, sizeof(cmd_MODE)) == 0)
        {
            result = cmd_sub_mode();
        }
        /* Eddystone frames */
        else if(memcmp(cmd_EDFRAME, cmd_tmp_upr, sizeof(cmd_EDFRAME)) == 0)
        {
            result = cmd_sub_edframe();
        }
        /* Eddystone state */
        else if(memcmp(cmd_EDSTATE, cmd_tmp_upr, sizeof(cmd_EDSTATE)) == 0)
        {
            /* If user just presses Enter after "EDstate". */
            if(data_array[sizeof(cmd_EDSTATE)] == '\0')
            {
                result = RESULT_OK;
                
                /* Current state is locked. */
                if(pBeaconConfig->LockState)
                {
                    SIMPLE_UART_PUTCONST("-> EDSTATE : locked\r\n");
                }
                /* Current state is unlocked. */
                else
                {
                    SIMPLE_UART_PUTCONST("-> EDSTATE : unlocked\r\n");
                }
            }
            /* If user input more than "EDstate", command is invalid. */
            else
            {
                /* result is still RESULT_ERROR. */
            }
        }
        /* Eddystone lock */
        else if(memcmp(cmd_EDLOCK, cmd_tmp_upr, sizeof(cmd_EDLOCK)) == 0)
        {
            result = cmd_sub_lock();
        }
        /* Eddystone unlock */
        else if(memcmp(cmd_EDUNLOCK, cmd_tmp_upr, sizeof(cmd_EDUNLOCK)) == 0)
        {
            result = cmd_sub_unlock();
        }
        /* Initialize */
        else if(memcmp(cmd_INIT, cmd_tmp_upr, sizeof(cmd_INIT)) == 0)
        {
            result = cmd_sub_init();
        }
        /* Command is invalid. */
        else
        {
            /* result is still RESULT_ERROR. */
        }
    
    }
    
    memset(data_array, 0, UART_MAX_DATA_LEN);
    
    /* If user input is invalid. */
    if(result == RESULT_ERROR)
    {
        SIMPLE_UART_PUTCONST(STRING_CMD_ERROR);    
    }
    else if(result == RESULT_REJECTED)
    {
        SIMPLE_UART_PUTCONST(STRING_CMD_UPDATE_REJECTED);
    }
    
    return result;
}

/******************************************************************************
* Function Name: cmd_sub_uuid
*******************************************************************************
*
* Summary:
*   Function for changing the uuid.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_uuid()
{
    uint16_t i, j;
    uint8_t str_buf[UUID_TMP_BUFFER_40_BYTE];
    uint8_t rbuff[sizeof(DefaultBeaconConfig.uuid)];
    uint8_t *pPos = NULL;
    
    /* If the command is getting the current UUID setting. */
    if(data_array[sizeof(cmd_UUID)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> UUID: ");
    }
    /* If the command is updating the UUID setting. */
    else if(strlen((const char *)(data_array + sizeof(cmd_UUID))) == (SIZE_OF_PARAM_SPACE + SIZE_OF_UUID_PARAM))
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_UUID)] != ' ')
        {
            return RESULT_ERROR;
        }
        
        pPos = &data_array[sizeof(cmd_UUID) + SIZE_OF_PARAM_SPACE];
        
        /* Copy the parameter to another buffer, check and remove the hyphens. */
        j = 0;
        for(i = 0; i < SIZE_OF_UUID_PARAM; i++)
        {
            /* Don't copy characters in hyphen positions. */
            if((UUID_STR_HYPHEN_POS_1 == i) || 
               (UUID_STR_HYPHEN_POS_2 == i) || 
               (UUID_STR_HYPHEN_POS_3 == i) || 
               (UUID_STR_HYPHEN_POS_4 == i))
            {
                /* If the character in hyphen positions isn't a hyphen. */
                if(pPos[i] != '-')
                {
                    return RESULT_ERROR;
                }
            }
            /* Copy other characters to the buffer. */
            else
            {
                str_buf[j++] = pPos[i];
            }
        }
        
        /* Convert and check the parameter. */
        if(hextobin(rbuff, str_buf, UUID_BYTES) == RESULT_ERROR)
        {
            return RESULT_ERROR;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_UUID, rbuff);
        SIMPLE_UART_PUTCONST("-> New UUID: ");
    }
    /* Length of the new UUID is invalid. */
    else
    {
        return RESULT_ERROR;
    }
    
    j = 0;
    for(i = 0; i < UUID_BYTES; i++)
    {
        if((UUID_HYPHEN_POS_1 == i) || \
           (UUID_HYPHEN_POS_2 == i) || \
           (UUID_HYPHEN_POS_3 == i) || \
           (UUID_HYPHEN_POS_4 == i))
        {
            str_buf[i * SIZE_OF_BYTE_TO_02X + j] = '-';
            j++;
        }
        sprintf((char *)&str_buf[i * SIZE_OF_BYTE_TO_02X + j], "%02X", pBeaconConfig->uuid[i]);
    }
    SIMPLE_UART_PUTCONST((char *)str_buf);
    SIMPLE_UART_PUTCONST("\r\n");

    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_ednid
*******************************************************************************
*
* Summary:
*   Function for changing the 10-byte ID Namespace of Eddystone-UID.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_ednid(void)
{ 
    int index;
    uint8_t *p;
    uint8_t tmp_nid[NID_BYTES]; 
    uint16_t len = strlen((const char *)(data_array + sizeof(cmd_EDNID)));
    uint8_t str[SIZE_OF_BYTE_TO_02X * NID_BYTES + SIZE_OF_NUL];

    /* If the command is getting the current NID setting. */
    if(len == 0)
    {
        SIMPLE_UART_PUTCONST("-> Eddystone-UID NID : "); 
    }
    /* If the command is setting the NID. */
    else if(len == SIZE_OF_BYTE_TO_02X * NID_BYTES + SIZE_OF_PARAM_SPACE)
    {
        p = data_array + sizeof(cmd_EDNID);
        /* The next char should be a space. */
        if(*p++ != ' ')
        {
            return RESULT_ERROR;
        }
        
        /* Convert and check the parameter. */
        if(hextobin(tmp_nid, p, NID_BYTES) == RESULT_ERROR)
        {
            return RESULT_ERROR;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_NID, tmp_nid);
        SIMPLE_UART_PUTCONST("-> New Eddystone-UID NID : "); 
    }
    /* The length is not right. */
    else
    {
        return RESULT_ERROR;
    }
    
    /* Convert nid to string. */
    for(index = 0; index < NID_BYTES; index++)
    {
        sprintf((char *)(str + SIZE_OF_BYTE_TO_02X * index), "%02X", pBeaconConfig->nid[index]);
    }
    
    SIMPLE_UART_PUTCONST((char *)str);
    SIMPLE_UART_PUTCONST("\r\n");
    
    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_edbid
*******************************************************************************
*
* Summary:
*   Function for changing the 6-byte ID Instance of Eddystone-UID.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_edbid(void)
{
    int      index;
    uint8_t  *p;
    uint8_t  tmp_bid[BID_BYTES];
    uint16_t len = strlen((const char *)(data_array + sizeof(cmd_EDBID)));
    uint8_t  str[SIZE_OF_BYTE_TO_02X * BID_BYTES + SIZE_OF_NUL];

    /* If the command is getting the current BID setting. */
    if(len == 0)
    {
        SIMPLE_UART_PUTCONST("-> Eddystone-UID BID : ");
    }
    /* If the command is setting the BID. */
    else if(len == SIZE_OF_BYTE_TO_02X * BID_BYTES + SIZE_OF_PARAM_SPACE)
    {
        p = data_array + sizeof(cmd_EDBID);
        /* The next char should be a space. */
        if(*p++ != ' ')
        {
            return RESULT_ERROR;
        }
        
        /* Convert and check the parameter. */
        if(hextobin(tmp_bid, p, BID_BYTES) == RESULT_ERROR)
        {
            return RESULT_ERROR;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_BID, tmp_bid);
        SIMPLE_UART_PUTCONST("-> New Eddystone-UID BID : "); 
    }
    /* The length is not right. */
    else
    {
        return RESULT_ERROR;
    }
    
    /* Convert bid to string. */
    for(index = 0; index < BID_BYTES; index++)
    {
        sprintf((char *)(str + SIZE_OF_BYTE_TO_02X * index), "%02X", pBeaconConfig->bid[index]);
    }
    
    SIMPLE_UART_PUTCONST((char *)str);
    SIMPLE_UART_PUTCONST("\r\n");
    
    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_eduri
*******************************************************************************
*
* Summary:
*   Read/Write of the URI Data for Eddystone-URL. The parameter can be a hex string 
*   or a URL string([protocol]://[domain]/[path]). The hex string is not case 
*   sensitive, the protocol and the domain of the URL string are not case sensitive, 
*   either. But the path of the URL string is case sensitive.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
static uint16_t cmd_sub_eduri(void)
{
    const uint8_t http[] = {'h', 't', 't', 'p'};
    /* Used to translate uri. */
    const char *url_PREFIX[] = {"://www.", "s://www.", "://", "s://"};
    const char *url_SUFFIX[] = {".com/", ".org/", ".edu/", ".net/", ".info/", ".biz/", ".gov/",
                                ".com",  ".org",  ".edu",  ".net",  ".info",  ".biz",  ".gov" };
    int      i;
    uint8_t  isHumanReadable = true;
    
    /* If the command is getting the current uri setting. */
    if(*(data_array + sizeof(cmd_EDURI)) == '\0')
    {
        SIMPLE_UART_PUTCONST("-> Eddystone-URL URI Data :");
    }
    /* If the command is setting the uri. */
    else
    {
        uint8_t tmp_Uri_Length = 0;
        uint8_t tmp_Uri_Data[EDDYSTONE_URI_DATA_MAX_LEN]; 
        uint8_t *pStartofURL;  /* Mark this starting position after a space in data_array. */
        uint8_t *p;
    
        /* If the space is lost. */
        if(data_array[sizeof(cmd_EDURI)] != ' ')
        {
            return RESULT_ERROR; 
        }
        
        pStartofURL = data_array + sizeof(cmd_EDURI) + SIZE_OF_PARAM_SPACE;
        
        /* If the parameter is hexadecimal format */
        if(*pStartofURL == '0') 
        {
            uint16_t tmp_result;
            
            isHumanReadable = false;
            
            p = pStartofURL;
            /* Check the prefix. */
            p++;        /* Skip the char '0'. */
            /* The head should be 00/01/02/03 */
            if((*p >= '0') && (*p <= '3')) 
            {
                tmp_Uri_Data[tmp_Uri_Length++] = *p++ - '0'; 
            }
            /* not start with "00" "01" "02" "03". */ 
            else
            {
                return RESULT_ERROR;
            }
            
            for( ; (*p != '\0') && (tmp_Uri_Length < EDDYSTONE_URI_DATA_MAX_LEN); p += SIZE_OF_BYTE_TO_02X)
            {
                tmp_result = twoaton(p);
                /* If the subsequent two characters is not in hex format. */
                if(tmp_result == TWOATON_PARAM_ERROR)
                {
                    return RESULT_ERROR;
                }
                /* It is a common character, or it's in the suffix table. */
                if( ((tmp_result > CHAR_LOWER_BOUND) && (tmp_result < CHAR_UPPER_BOUND)) ||
                    (tmp_result < KINDS_OF_URL_SUFFIX) )
                {
                    tmp_Uri_Data[tmp_Uri_Length++] = tmp_result;
                }
                /* The character is invalid. */
                else
                {
                    return RESULT_ERROR;
                }
            }
            /* The parameter is too long. Overflow. */
            if(*p != '\0')
            {
                return RESULT_ERROR;
            }
        }
        /* The parameter is not hexadecimal format, try resolving with ASCII URL format */
        else
        {
            uint16_t len;
            
            /* The protocol and the domain name of the url is not case sensitive, 
             * convert them to lowercase, they locates before '\0' or the third '/'. */
            for(p = pStartofURL, i = 0; (*p != '\0') && (i < SLASH_COUNT_3); p++)
            {
                if(*p == '/')
                {
                    i++;
                    continue;
                }
                *p = ((*p >= 'A') && (*p <= 'Z')) ? (*p + ('a' - 'A')) : *p;
            }
            
            /* If it doesn't start with "http", the parameter is wrong. */
            if(memcmp(pStartofURL, http, sizeof(http)) != 0 )
            {
                return RESULT_ERROR;
            }
            
            /* Try to find the matching prefix. */
            p = pStartofURL + sizeof(http);
            for(i = 0; i < KINDS_OF_URL_PREFIX; i++)
            {
                len = strlen((char *)url_PREFIX[i]);
                /* If we find it. */
                if(memcmp(p, url_PREFIX[i], len) == 0)
                {
                    tmp_Uri_Data[tmp_Uri_Length++] = i;
                    /* Skip the prefix we have found. */
                    p += len;
                    break;
                }
            }
            /* If we do not find the matching prefix. */ 
            if(i == KINDS_OF_URL_PREFIX)
            {
                return RESULT_ERROR;
            }

            /* Now, deal with the remaining content after the prefix. */  
            while((*p != '\0') && (tmp_Uri_Length < EDDYSTONE_URI_DATA_MAX_LEN))
            {
                /* The char is not between 0x20 and 0x7f */
                if((*p <= CHAR_LOWER_BOUND) || (*p >= CHAR_UPPER_BOUND))
                { 
                    return RESULT_ERROR;
                }
                /* Try to find if it is in the matching suffix table. */
                else if(*p == '.')
                {
                    for(i = 0; i < KINDS_OF_URL_SUFFIX; i++)
                    {
                        len = strlen((char *)url_SUFFIX[i]);
                        /* If we find it. */
                        if(memcmp(p, url_SUFFIX[i], len) == 0)
                        {
                            tmp_Uri_Data[tmp_Uri_Length++] = i;
                            /* Skip the suffix we have just found. */
                            p += len;
                            break;
                        }
                    }
                    /* No matching suffix. It's just a '.' */
                    if(i == KINDS_OF_URL_SUFFIX)
                    {
                        tmp_Uri_Data[tmp_Uri_Length++] = *p;
                        p++;
                    }
                }
                /* When it is a legal character but '.', 
                 * we just put it in the array tmp_Uri_Data. */ 
                else
                {
                    tmp_Uri_Data[tmp_Uri_Length++] = *p++;
                }
            }
            /* The parameter is too long. Overflow. */
            if(*p != '\0')
            {
                return RESULT_ERROR;
            }
        }
        
        /* The updating operation is not allowed in the locked state. */
        if(pBeaconConfig->LockState)
        {
            return RESULT_REJECTED;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_URILENGTH, &tmp_Uri_Length);
        ConfigRW_UpdateConfig(CONFIG_TYPE_URIDATA, tmp_Uri_Data);
        
        /* update URI Data in the GATT database of the GATT Server. */
        CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
        cyBle_attValuesLen[ATT_VALUES_LEN_INDEX_URIDATA].actualLength = pBeaconConfig->URI_Length;
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE;
        UpdateHandle.value.val  = pBeaconConfig->URI_Data;
        UpdateHandle.value.len  = pBeaconConfig->URI_Length;
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        SIMPLE_UART_PUTCONST("-> New Eddystone-URL URI Data :");
    }
    
    /* Output the uri in ASCII format. */
    if(isHumanReadable)
    {
        SIMPLE_UART_PUTCONST("http");
        SIMPLE_UART_PUTCONST((char *)url_PREFIX[pBeaconConfig->URI_Data[0]]); 

        for(i = 1; i < pBeaconConfig->URI_Length; i++)
        {
            /* If it is a suffix. */
            if(pBeaconConfig->URI_Data[i] < KINDS_OF_URL_SUFFIX)
            {
                SIMPLE_UART_PUTCONST((char *)url_SUFFIX[pBeaconConfig->URI_Data[i]]);
            }
            /* just a common character */
            else
            {
                SIMPLE_UART_PUT_CHAR(pBeaconConfig->URI_Data[i]);
            }
        }
        SIMPLE_UART_PUTCONST("\r\n");
    }
    /* Output the uri in Hexadecimal format. */
    else
    {
        char str[SIZE_OF_BYTE_TO_02X + SIZE_OF_NUL];
        
        for(i = 0; i < pBeaconConfig->URI_Length; i++)
        {
            sprintf((char *)str, "%02X", pBeaconConfig->URI_Data[i]);
            SIMPLE_UART_PUTCONST(str);
        }
        
        SIMPLE_UART_PUTCONST("\r\n");
    }
    
    return RESULT_OK;
} 

/******************************************************************************
* Function Name: cmd_sub_major
*******************************************************************************
*
* Summary:
*   Function for changing the major.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_major()
{
    uint16_t i;
    uint16_t result = RESULT_OK;
    char str[TMP_BUFFER_4_BYTE];
    uint8_t m[TMP_BUFFER_4_BYTE];
    uint16_t major;
    
    /* If the command is getting the current MAJOR setting. */
    if(data_array[sizeof(cmd_MAJOR)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> MAJOR: ");
    }
    /* If the command is updating the MAJOR setting. */
    else if(strlen((const char *)(data_array + sizeof(cmd_MAJOR))) == (SIZE_OF_PARAM_SPACE + SIZE_OF_MAJOR_PARAM))
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_MAJOR)] != ' ')
        {
            return RESULT_ERROR;
        }
        /* check character 0-9, a-f, A-F */
        for(i = 0; i < SIZE_OF_MAJOR_PARAM; i++)
        {
            /* Characters in the new MAJOR must be '0' - 'F'/'f', convert them from character to digit. */
            if((m[i] = atox(data_array[i + sizeof(cmd_MAJOR) + SIZE_OF_PARAM_SPACE])) == ATOX_PARAM_ERROR)
            {
                result = RESULT_ERROR;
                break;          /* Handle the error. */
            }
        }
        /* If some character of the new MAJOR is invalid. */
        if(result != RESULT_OK)
        {
            return result;
        }

        major = BYTE_H4_L4_BIT_SWITCH(&m[MAJOR_HIGH_BYTE_OFFSET]);
        major = (major << BIT_PER_BYTE) | BYTE_H4_L4_BIT_SWITCH(&m[MAJOR_LOW_BYTE_OFFSET]);
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_MAJOR, &major);
        SIMPLE_UART_PUTCONST("-> New MAJOR: ");
    }
    /* Length of the new MAJOR is invalid. */
    else
    {
        return RESULT_ERROR;
    }
    sprintf(str, "%04X", pBeaconConfig->major);
    SIMPLE_UART_PUTCONST(str);
    SIMPLE_UART_PUTCONST("\r\n");

    return result;
}

/******************************************************************************
* Function Name: cmd_sub_minor
*******************************************************************************
*
* Summary:
*   Function for changing the minor.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_minor()
{
    uint16_t i;
    uint16_t result = RESULT_OK;
    char str[TMP_BUFFER_4_BYTE];
    uint8_t m[TMP_BUFFER_4_BYTE];
    uint16_t minor;

    /* If the command is getting the current MINOR setting. */
    if(data_array[sizeof(cmd_MINOR)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> MINOR: ");
    }
    /* If the command is updating the MINOR setting. */
    else if(strlen((const char *)(data_array + sizeof(cmd_MINOR))) == (SIZE_OF_PARAM_SPACE + SIZE_OF_MINOR_PARAM))
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_MINOR)] != ' ')
        {
            return RESULT_ERROR;
        }
        /* check character 0-9, a-f, A-F */
        for(i = 0; i < SIZE_OF_MINOR_PARAM; i++) 
        {
            /* Characters in the new MINOR must be '0' - 'F'/'f', convert them from character to digit. */
            if((m[i] = atox(data_array[i + sizeof(cmd_MINOR) + SIZE_OF_PARAM_SPACE])) == ATOX_PARAM_ERROR) 
            {
                result = RESULT_ERROR;
                break;          /* Handle the error. */
            }
        }
        /* If some character of the new MINOR is invalid. */
        if(result != RESULT_OK)
        {
            return result;
        }
           
        minor = BYTE_H4_L4_BIT_SWITCH(&m[MINOR_HIGH_BYTE_OFFSET]);
        minor = (minor << BIT_PER_BYTE) | BYTE_H4_L4_BIT_SWITCH(&m[MINOR_LOW_BYTE_OFFSET]);
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_MINOR, &minor);
        SIMPLE_UART_PUTCONST("-> New MINOR: ");
    }
    /* Length of the new MINOR is invalid. */
    else
    {
        return RESULT_ERROR;
    }
    sprintf(str, "%04X",  pBeaconConfig->minor);
    SIMPLE_UART_PUTCONST(str);
    SIMPLE_UART_PUTCONST("\r\n");
    
    return result;
}

/******************************************************************************
* Function Name: cmd_sub_rssi
*******************************************************************************
*
* Summary:
*   Function for changing the rssi of BLEBeacon mode.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_rssi()
{
    uint16_t i;
    uint16_t result = RESULT_OK;
    char str[] = {'\0', '\0', '\0', '\0'};
    int32_t rssi = 0;
    uint16_t len = strlen((const char *)(data_array + sizeof(cmd_RSSI)));

    /* If the command is getting the current RSSI setting. */
    if(len == 0)
    {
        SIMPLE_UART_PUTCONST("-> RSSI in dBm: ");
    }
    /* Length of the new RSSI is invalid. */
    else if(RSSI_INVALID_PARAM_LEN == len)
    {
        return RESULT_ERROR;
    }
    else
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_RSSI)] != ' ')
        {
            return RESULT_ERROR;
        }
        /* Copy the new RSSI to str and check */
        for(i = 0; i < (len - SIZE_OF_PARAM_SPACE); i++)
        {
            /* Generally, characters should be '0' - '9'. But the first character can be '-' */
            if(((data_array[i + sizeof(cmd_RSSI) + SIZE_OF_PARAM_SPACE] >= '0')  && \
                (data_array[i + sizeof(cmd_RSSI) + SIZE_OF_PARAM_SPACE] <= '9')) || \
                ((0 == i) && (data_array[i + sizeof(cmd_RSSI) + SIZE_OF_PARAM_SPACE] == '-')))
            {
                str[i] = data_array[i + sizeof(cmd_RSSI) + SIZE_OF_PARAM_SPACE];
            }
            else
            {
                return RESULT_ERROR;
            }
        }
        /* in case str is "-" with no '0' - '9'. */
        if((RSSI_MINUS_INVALID_PARAM_LEN == len) && ('-' == str[RSSI_OFFSET_OF_MINUS]))
        {
            return RESULT_ERROR;
        }
        rssi = (int32_t)atoi(str);

        /* check validity of the new rssi value */
        if((rssi >= RSSI_MIN_VALID_VALUE) && (rssi <= RSSI_MAX_VALID_VALUE))
        {
            rssi = (int8_t)(rssi & RSSI_LOW_8BIT_KEEP_FLAG);

            ConfigRW_UpdateConfig(CONFIG_TYPE_RSSI, (int8_t *)&rssi);
            SIMPLE_UART_PUTCONST("-> New RSSI in dBm: ");
        }
        /* the new rssi value is invalid */
        else
        {
            return RESULT_ERROR;
        }
    }
    
    sprintf(str, "%d", (int8_t)pBeaconConfig->rssi);
    SIMPLE_UART_PUTCONST(str);
    SIMPLE_UART_PUTCONST("\r\n");
    
    return result;
}

/******************************************************************************
* Function Name: cmd_sub_itrvl
*******************************************************************************
*
* Summary:
*   Function for changing the interval of BLEBeacon advertising.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_itrvl(void)
{
    uint16_t i;
    uint16_t result = RESULT_OK;
    char str[] = {'\0', '\0', '\0', '\0', '\0', '\0'};
    uint32_t adv_interval = 0;
    uint16_t len = strlen((const char *)(data_array + sizeof(cmd_ITRVL)));
    
    /* If the command is getting the current ITRVL setting. */
    if(len == 0)
    {
        SIMPLE_UART_PUTCONST("-> BLEBeacon Advertise Interval in msec: ");
    }
    /* If the command is updating the ITRVL setting. */
    else if((len >= SIZE_OF_PARAM_SPACE + ITRVL_MIN_TIME_SIZE) && (len <= SIZE_OF_PARAM_SPACE + ITRVL_MAX_TIME_SIZE)) /* space(len=1) + value(len=3/4/5) */
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_ITRVL)] != ' ')
        {
            return RESULT_ERROR;
        }
        for(i = 0; i < (len - SIZE_OF_PARAM_SPACE); i++)
        {
            str[i] = data_array[i + SIZE_OF_PARAM_SPACE + sizeof(cmd_ITRVL)];
            if((str[i] < '0') || (str[i] > '9'))
            {
                return RESULT_ERROR;
            }
        }
        adv_interval = (uint32_t)atoi(str);
        /* Advertising interval between 100ms and 10.24s */
        if((adv_interval < ITRVL_MIN_TIME) || (adv_interval > ITRVL_MAX_TIME))
        {
            return RESULT_ERROR;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_ITRVL, (uint16_t *)&adv_interval);
        SIMPLE_UART_PUTCONST("-> New BLEBeacon Advertise Interval in msec: ");
    }
    /* Length of the new ITRVL is invalid. */
    else
    {
        return RESULT_ERROR;
    }
    
    sprintf(str, "%5ld", (long)pBeaconConfig->itrvl);
    SIMPLE_UART_PUTCONST(str);
    SIMPLE_UART_PUTCONST("\r\n");
    
    return result;
}

/******************************************************************************
* Function Name: cmd_sub_editrvl
*******************************************************************************
*
* Summary:
*   Function for changing the interval of Eddystone advertising.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
static uint16_t cmd_sub_editrvl(void)
{
    uint32_t adv_editrvl = 0;
    uint8_t *p;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
    char str[ITRVL_MAX_TIME_SIZE + SIZE_OF_NUL];
    
    /* If the command is getting the current EDITRVL setting. */
    if(*(data_array + sizeof(cmd_EDITRVL)) == '\0')
    {
        SIMPLE_UART_PUTCONST("-> Eddystone Advertise Interval in msec: ");
    }
    /* If the command is updating the EDITRVL setting. */
    else
    {
        p = data_array + sizeof(cmd_EDITRVL);
        
        /* There must be a space after the command. */
        if(*p != ' ')
        {
            return RESULT_ERROR;
        }
        p++;
        /* If the subsequent is nul. */
        if(*p == '\0')
        {
            return RESULT_ERROR;
        }
        /* Check if the following is numbers, and calculate the result. */
        for( ; *p != '\0'; p++)
        {
            /* If it is not a number. */
            if((*p < '0') || (*p > '9'))
            {
                return RESULT_ERROR;
            }
            /* If the result is big enough, it's not necessary to compute it. */
            else if(adv_editrvl > ITRVL_MAX_TIME) 
            {
                continue;
            }
            /* It is a correct number. */
            else
            {
                adv_editrvl = (adv_editrvl * 10) + (*p - '0');
            }
        }
        
        /* Advertising interval should be between 100ms and 10.24s or just 0. */
        if((adv_editrvl > 0) && (adv_editrvl < ITRVL_MIN_TIME))
        {
            adv_editrvl = ITRVL_MIN_TIME;
        }
        else if(adv_editrvl > ITRVL_MAX_TIME)
        {
            adv_editrvl = ITRVL_MAX_TIME;
        }
        
        /* The updating operation is not allowed in locked state. */
        if(pBeaconConfig->LockState)
        {
            return RESULT_REJECTED;
        }
        
        /* Update the editrvl configuration stored in the user flash. */
        ConfigRW_UpdateConfig(CONFIG_TYPE_URLPERIOD, (uint16_t *)(&adv_editrvl));
        
        /* Update Beacon Period in the GATT database of the GATT Server. */
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE;
        UpdateHandle.value.val  = (uint8_t *)(&(pBeaconConfig->URLPeriod));
        UpdateHandle.value.len  = sizeof(pBeaconConfig->URLPeriod);
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        SIMPLE_UART_PUTCONST("-> New Eddystone Advertise Interval in msec: ");
    }
    
    sprintf(str, "%5d", pBeaconConfig->URLPeriod);
    SIMPLE_UART_PUTCONST(str); 
    SIMPLE_UART_PUTCONST("\r\n"); 

    return RESULT_OK;
}

/*******************************************************************************
* Function Name: cmd_sub_edreset
********************************************************************************
* Summary:
*   Function for setting all characteristics to their initial values.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
*******************************************************************************/
static uint16_t cmd_sub_edreset(void)
{
    /* User inputs more than "edreset". */
    if(data_array[sizeof(cmd_EDRESET)] != '\0')
    {
        return RESULT_ERROR;
    }
    /* If the beacon is locked. */
    else if(pBeaconConfig->LockState)
    {
        return RESULT_REJECTED;
    }
    /* The length is right, and the beacon is unlocked. */
    else
    {
        ConfigRW_WriteReset();
        EddystoneURL_Config_UpdateAllAttrib();
        
        SIMPLE_UART_PUTCONST("-> Reset finished!\r\n");
    }
    
    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_txpwr
*******************************************************************************
*
* Summary:
*   Function for changing the tx power of BLEBeacon mode.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_txpwr(void)
{
    uint16_t i;
    uint16_t result = RESULT_OK;
    char str[] = {'\0', '\0', '\0', '\0'};
    int32_t txpwr = 0;
    uint16_t len = strlen((const char *)(data_array + sizeof(cmd_TXPWR)));
    
    /* If the command is getting the current TXPWR setting. */
    if(len == 0)
    {
        SIMPLE_UART_PUTCONST("-> TX power in dBm: ");
    }
    /* The command is updating the TXPWR setting. */
    else
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_TXPWR)] != ' ')
        {
            return RESULT_ERROR;
        }
        if((data_array[SIZE_OF_PARAM_SPACE + sizeof(cmd_TXPWR)]) == '-') /* minus */
        {
            /* accepted length can be length of " -18", " -6". */
            if((len != SIZE_OF_PARAM_SPACE + TXPWR_MINUS_VALID_LEN_1) && \
               (len != SIZE_OF_PARAM_SPACE + TXPWR_MINUS_VALID_LEN_2))
            {
                return RESULT_ERROR;
            }
            str[0] = data_array[SIZE_OF_PARAM_SPACE + sizeof(cmd_TXPWR)];
            for(i = TXPWR_MINUS_VALUE_OFFSET; i < len - SIZE_OF_PARAM_SPACE; i++) 
            {
                str[i] = data_array[i + SIZE_OF_PARAM_SPACE + sizeof(cmd_TXPWR)];
                /* Characters must be '0' - '9'. */
                if((str[i] < '0') || (str[i] >'9'))
                {
                    return RESULT_ERROR;
                }
            }
        }
        else /* plus */
        {
            /* accepted length can be length of " 3". */
            if(len != SIZE_OF_PARAM_SPACE + TXPWR_PLUS_VALID_VALUE_LEN)
            {
                return RESULT_ERROR;
            }
            str[0] = data_array[SIZE_OF_PARAM_SPACE + sizeof(cmd_TXPWR)];
            /* Characters must be '0' - '9'. */
            if((str[0] < '0') || (str[0] >'9'))
            {
                return RESULT_ERROR;
            }
        }
        txpwr = (int32_t)atoi(str);
        /* accepted values are -18, -12, -6, -3, -2, -1, 0 and 3 dBm */
        if ((TXPWR_ACCEPTED_VALUE_NEG_18 == txpwr) ||
            (TXPWR_ACCEPTED_VALUE_NEG_12 == txpwr) ||
            (TXPWR_ACCEPTED_VALUE_NEG_6 == txpwr)  ||
            (TXPWR_ACCEPTED_VALUE_NEG_3 == txpwr)  ||
            (TXPWR_ACCEPTED_VALUE_NEG_2 == txpwr)  ||
            (TXPWR_ACCEPTED_VALUE_NEG_1 == txpwr)  ||
            (TXPWR_ACCEPTED_VALUE_0 == txpwr)      ||
            (TXPWR_ACCEPTED_VALUE_3 == txpwr))
        {
            ConfigRW_UpdateConfig(CONFIG_TYPE_TXPWR, (int8_t *)&txpwr);
            SIMPLE_UART_PUTCONST("-> New TX power in dBm: ");
        }
        /* Value of the new TXPWR is invalid. */
        else
        {
            return RESULT_ERROR;
        }
    }
    
    sprintf(str, "%d", (int)pBeaconConfig->txpwr);
    SIMPLE_UART_PUTCONST(str);
    SIMPLE_UART_PUTCONST("\r\n");
    
    return result;
}

/******************************************************************************
* Function Name: cmd_sub_edtxpwr
*******************************************************************************
*
* Summary:
*   Function for getting/setting the radio TX power levels for Eddystone.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
static uint16_t cmd_sub_edtxpwr(void)
{
    uint8_t i;
    uint8_t j;
    uint8_t *p;
    uint8_t length;
    char str[] = {0, 0, 0, 0, 0, 0};
    int8_t tmp_TXPowerLevels[ES_TX_POWER_MODE_COUNT];
    /* accepted values are -18, -12, -6, -3, -2, -1, 0 and 3 dBm */
    const char *strTab[] = {" -18", " -12", " -6", " -3", " -2", " -1", " 0", " 3"};
    const int8_t valueTab[] = {TXPWR_ACCEPTED_VALUE_NEG_18, TXPWR_ACCEPTED_VALUE_NEG_12,
                               TXPWR_ACCEPTED_VALUE_NEG_6,  TXPWR_ACCEPTED_VALUE_NEG_3, 
                               TXPWR_ACCEPTED_VALUE_NEG_2,  TXPWR_ACCEPTED_VALUE_NEG_1, 
                               TXPWR_ACCEPTED_VALUE_0,      TXPWR_ACCEPTED_VALUE_3      };
    
    /* If the command is getting the current radio TX power levels setting for Eddystone. */
    if(*(data_array + sizeof(cmd_EDTXPWR)) == '\0')
    {
        SIMPLE_UART_PUTCONST("-> Eddystone Radio TX Power Levels :");
    }
    /* The command is updating the radio TX power levels setting for Eddystone. */
    else
    {
        /* Starting to match the input with strTab. */
        p = data_array + sizeof(cmd_EDTXPWR);
        for(i = 0; i < ES_TX_POWER_MODE_COUNT; i++)
        {
            for(j = 0; j < TXPWR_ACCEPTED_COUNT; j++)
            {
                length = strlen(strTab[j]);
                /* We find it in the strTab. */
                if(memcmp(strTab[j], p, length) == 0)
                {
                    tmp_TXPowerLevels[i] = valueTab[j]; 
                    p += length;    /* skip the matching part */  
                    break;
                }
            }
            /* If no matching str has been found. */
            if(j == TXPWR_ACCEPTED_COUNT)
            {
                return RESULT_ERROR;
            }
        }
        /* The input is too long. */
        if(*p != '\0')
        {
            return RESULT_ERROR;
        }
        /* The array is in an ascending order. */ 
        if( (tmp_TXPowerLevels[0] < tmp_TXPowerLevels[1]) && \
            (tmp_TXPowerLevels[1] < tmp_TXPowerLevels[2]) && \
            (tmp_TXPowerLevels[2] < tmp_TXPowerLevels[3]) )
        {
            /* The updating operation is not allowed in locked state. */
            if(pBeaconConfig->LockState)
            {
                return RESULT_REJECTED;
            }
        
            ConfigRW_UpdateConfig(CONFIG_TYPE_EDTXPWR, &tmp_TXPowerLevels);
            SIMPLE_UART_PUTCONST("-> New Eddystone Radio TX Power Levels :");
        }
        else
        {
            return RESULT_ERROR;
        }
    }

    /* Output the Eddystone Radio TX Power Levels */
    for(i = 0; i < ES_TX_POWER_MODE_COUNT; i++)
    {
        sprintf(str, " %d", pBeaconConfig->TXPowerLevels[i]);
        SIMPLE_UART_PUTCONST(str);
    }

    SIMPLE_UART_PUTCONST("\r\n");

    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_edadpwr
*******************************************************************************
*
* Summary:
*   Function for getting/setting the Advertised TX Power Levels for Eddystone.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
static uint16_t cmd_sub_edadpwr(void)
{
    uint8_t i;
    uint8_t index = 0; 
    uint8_t *p;
    int32_t res;   /* Store the temp result of the conversion. */
    CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
    char str[] = {0, 0, 0, 0, 0, 0};
    int8_t tmp_Adv_TXPowerLevels[ES_TX_POWER_MODE_COUNT];

    /* If the command is getting the current Advertised TX Power Levels setting for Eddystone. */
    if(*(data_array + sizeof(cmd_EDADPWR)) == '\0') 
    {
        SIMPLE_UART_PUTCONST("-> Eddystone Advertised TX Power Levels :");
    }
    /* The command is updating the Advertised TX Power Levels setting for Eddystone. */
    else
    {
        p = data_array + sizeof(cmd_EDADPWR);
        /* Convert the input string into number. */ 
        while((*p != '\0') && (index < ES_TX_POWER_MODE_COUNT))
        {
            /* Every part of the input should be " x" */
            res = 0;
            if(*p++ != ' ') /* The space is needed. */
            
            {
                return RESULT_ERROR;
            }
            /* record the length of the input number. */
            i = 0;
            /* Move p pointing to the next space or '\0', and at the same time verify the character. */
            while((*p != ' ') && (*p != '\0'))
            {
                if(((*p == '-') && (i == 0))  ||        /* The first character is '-'. */
                   ((*p >= '0') && (*p <= '9')))        /* The char is in 0~9. */
                {
                    p++;
                    i++;
                }
                /* The input is not correct. */
                else
                {
                    return RESULT_ERROR;
                }
            }
            if((i > ADPWR_INPUT_LONGEST_NUM) ||         /* The longest format : -xxx. */
               ((i == 1) && (*(p - i) == '-')))         /* The input num is only a '-'. */
            {
                return RESULT_ERROR;
            }
            res = atoi((char *)(p - i));
            /* If it is not in the range -100~20. */
            if((res < ADPWR_LOWER_BOUND) || (res > ADPWR_UPPER_BOUND))
            {
                return RESULT_ERROR;
            }

            tmp_Adv_TXPowerLevels[index++] = res; 
        }
        /* There are more or less than 4 parts of input. */ 
        if((*p != '\0') || (index != ES_TX_POWER_MODE_COUNT))
        {
            return RESULT_ERROR;
        }
        /* The array is in an ascending order. */
        if((tmp_Adv_TXPowerLevels[0] < tmp_Adv_TXPowerLevels[1]) &&
           (tmp_Adv_TXPowerLevels[1] < tmp_Adv_TXPowerLevels[2]) &&
           (tmp_Adv_TXPowerLevels[2] < tmp_Adv_TXPowerLevels[3]))
        {
            /* The updating operation is not allowed in locked state. */
            if(pBeaconConfig->LockState)
            {
                return RESULT_REJECTED;
            }
        
            ConfigRW_UpdateConfig(CONFIG_TYPE_EDADPWR, &tmp_Adv_TXPowerLevels);
            
            /* Update TX Power Levels in the GATT database of the GATT Server. */
            UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE;
            UpdateHandle.value.val  = (uint8_t *)(pBeaconConfig->Adv_TXPowerLevels);
            UpdateHandle.value.len  = ES_TX_POWER_MODE_COUNT;
            CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                           WRITE_ATTRIB_OFFSET_0, 
                                           NULL, 
                                           CYBLE_GATT_DB_LOCALLY_INITIATED);
            
            SIMPLE_UART_PUTCONST("-> New Eddystone Advertised TX Power Levels :");
        }
        else 
        {
            return RESULT_ERROR; 
        }
    }

    /* Output the Eddystone Advertised TX Power Levels */
    for(i = 0; i < ES_TX_POWER_MODE_COUNT; i++)
    {
        sprintf(str, " %d", pBeaconConfig->Adv_TXPowerLevels[i]); 
        SIMPLE_UART_PUTCONST(str);
    }

    SIMPLE_UART_PUTCONST("\r\n");

    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_edtxmode
*******************************************************************************
*
* Summary:
*   Function for getting/setting the TX Power mode for Eddystone.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
static uint16_t cmd_sub_edtxmode(void)
{
    uint8_t *p;
    uint8_t NewMode;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
    uint8_t len;
    const char *str[] = {"LOWEST", "LOW", "MEDIUM", "HIGH"};

    /* If the command is getting the current TX Power mode for Eddystone. */
    if(data_array[sizeof(cmd_EDTXMODE)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> Eddystone TX Power Mode : ");
    }
    /* The command is updating the TX Power mode for Eddystone. */
    else if(data_array[sizeof(cmd_EDTXMODE)] == ' ')
    {
        p = data_array + sizeof(cmd_EDTXMODE) + SIZE_OF_PARAM_SPACE;
        strupr((char *)p);
        
        /* look for the matching parameter */
        for(NewMode = 0; NewMode < ES_TX_POWER_MODE_COUNT; NewMode++)
        {
            len = strlen(str[NewMode]);
            /* find the matching parameter */
            if(memcmp(p, str[NewMode], len) == 0)
            {
                p += len;
                break;
            }
        }
        /* If no valid parameter is found, or the input is too long. */
        if((NewMode == ES_TX_POWER_MODE_COUNT) || (*p != '\0'))
        {
            return RESULT_ERROR; 
        }
        
        /* The updating operation is not allowed in locked state. */
        if(pBeaconConfig->LockState)
        {
            return RESULT_REJECTED;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_TXPMODE, &NewMode);
        
        /* update TX Power Mode in the GATT database of the GATT Server. */
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE;
        UpdateHandle.value.val  = &(pBeaconConfig->TXPowerMode);
        UpdateHandle.value.len  = sizeof(pBeaconConfig->TXPowerMode);
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        SIMPLE_UART_PUTCONST("-> New Eddystone TX Power Mode : ");
    }
    /* The char after the command is not space, format is wrong. */
    else
    {
        return RESULT_ERROR;
    }
    
    /* Output the Eddystone Advertised TX Power Levels */
    SIMPLE_UART_PUTCONST(str[pBeaconConfig->TXPowerMode]);
    SIMPLE_UART_PUTCONST("\r\n");

    return RESULT_OK;
}

/****************************************************************************** 
* Function Name: cmd_sub_coid
*******************************************************************************
*
* Summary:
*   Function for changing the company id.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_coid(void)
{
    uint16_t i;
    uint16_t result = RESULT_OK;
    char str[TMP_BUFFER_4_BYTE];
    uint8_t m[TMP_BUFFER_4_BYTE];
    uint16_t company_id = 0;

    /* If the command is getting the current Company ID setting. */
    if(data_array[sizeof(cmd_COID)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> Company ID: ");
    }
    /* If the command is updating the Company ID setting. */
    else if(strlen((const char *)(data_array + sizeof(cmd_COID))) == (SIZE_OF_PARAM_SPACE + SIZE_OF_COID_PARAM))
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_COID)] != ' ')
        {
            return RESULT_ERROR;
        }
        /* check character 0-9, a-f, A-F */
        for(i = 0; i < SIZE_OF_COID_PARAM; i++) 
        {
            /* Characters in the new COID must be '0' - 'F'/'f', convert them from character to digit. */
            if((m[i] = atox(data_array[i + sizeof(cmd_COID) + SIZE_OF_PARAM_SPACE])) == ATOX_PARAM_ERROR) 
            {
                result = RESULT_ERROR;
                break;          /* Handle the error. */
            }
        }
        /* If some character of the new COID is invalid. */
        if(result != RESULT_OK)
        {
            return result;
        }

        company_id = BYTE_H4_L4_BIT_SWITCH(&m[COID_HIGH_BYTE_OFFSET]);
        company_id = (company_id << BIT_PER_BYTE) | BYTE_H4_L4_BIT_SWITCH(&m[COID_LOW_BYTE_OFFSET]);

        ConfigRW_UpdateConfig(CONFIG_TYPE_COMID, (uint16_t *)&company_id);
        SIMPLE_UART_PUTCONST("-> New Company ID: ");
    }
    /* Length of the new Company ID is invalid. */
    else
    {
        return RESULT_ERROR;
    }

    sprintf(str, "%04X", pBeaconConfig->comID);
    SIMPLE_UART_PUTCONST(str);
    SIMPLE_UART_PUTCONST("\r\n");

    return result;
}

/******************************************************************************
* Function Name: cmd_sub_exsens
*******************************************************************************
*
* Summary:
*   Function for changing the sensor's mode.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_exsens(void)
{
    char *str;
    uint16_t len = strlen((const char *)(data_array + sizeof(cmd_EXSENS)));
    const char on[]  = {'O', 'N'};
    const char off[] = {'O', 'F', 'F'};
    uint8_t sensor;
    
    /* If the command is getting the current SENSOR setting. */
    if(len == 0)
    {
        SIMPLE_UART_PUTCONST("-> Sensor mode : ");
    }
    /* If the command is updating the SENSOR setting. */
    else if(((SIZE_OF_PARAM_SPACE + sizeof(on)) == len) || \
            ((SIZE_OF_PARAM_SPACE + sizeof(off)) == len)) /* space(len=1) + value(len=2/3) */
    {
        /* There must be a space after the command. */
        if(data_array[sizeof(cmd_EXSENS)] != ' ')
        {
            return RESULT_ERROR;
        }
        
        str = (char *)(&data_array[sizeof(cmd_EXSENS) + SIZE_OF_PARAM_SPACE]);
        /* Convert the input into Capital form. */
        strupr(str);
        
        /* check if parameter is off */
        if((memcmp(str, off, sizeof(off)) == 0) && \
           ((SIZE_OF_PARAM_SPACE + sizeof(off)) == len))
        {
            sensor = false;
        }
        /* check if parameter is on */
        else if((memcmp(str, on, sizeof(on)) == 0) && \
                ((SIZE_OF_PARAM_SPACE + sizeof(on)) == len))
        {
            sensor = true;
        }
        /* The new SENSOR setting is neither "on" nor "off". */
        else
        {
            return RESULT_ERROR;
        }
        
        ConfigRW_UpdateConfig(CONFIG_TYPE_SENSOR, (int8_t *)&sensor);
        SIMPLE_UART_PUTCONST("-> New Sensor mode : ");
    }
    /* Length of the new SENSOR is invalid. */
    else
    {
        return RESULT_ERROR;
    }

    if(pBeaconConfig->sensor)
    {
        SIMPLE_UART_PUTCONST("on");
    }
    else
    {
        SIMPLE_UART_PUTCONST("off");
    }
    SIMPLE_UART_PUTCONST("\r\n");

    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_mode
*******************************************************************************
*
* Summary:
*   Function for getting or changing the mode, bleBeacon or Eddystone.
*
* Parameters:
*   None
*  
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_mode(void)
{
    uint8_t *p;
    const char bleBeacon[] = {' ', 'B', 'L', 'E', 'B', 'E', 'A', 'C', 'O', 'N', '\0'};
    const char eddystone[] = {' ', 'E', 'D', 'D', 'Y', 'S', 'T', 'O', 'N', 'E', '\0'};
    const char edtest[]    = {' ', 'E', 'D', 'T', 'E', 'S', 'T', '\0'};
    int8_t mode;
    
    /* If the command is getting the current mode setting. */
    if(data_array[sizeof(cmd_MODE)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> Mode : ");
    }
    /* If the command is updating the MODE setting. */
    else
    {
        p = data_array + sizeof(cmd_MODE);
        
        /* Convert lower case to upper case. */
        strupr((char *)p);
        
        /* The mode setting is changed to bleBeacon. */
        if(memcmp(p, bleBeacon, sizeof(bleBeacon)) == 0)
        {
            mode = ADV_MODE_BLEBEACON;
        }
        /* The mode setting is changed to eddystone. */
        else if(memcmp(p, eddystone, sizeof(eddystone)) == 0)
        {
            mode = ADV_MODE_EDDYSTONE;
        }
        /* The mode setting is changed to eddystonetest. */
        else if(memcmp(p, edtest, sizeof(edtest)) == 0)
        {
            mode = ADV_MODE_EDDYSTONETEST;
        }
        /* The input mode is not right. */
        else
        {
            return RESULT_ERROR;
        }

        ConfigRW_UpdateConfig(CONFIG_TYPE_MODE, &mode);
        SIMPLE_UART_PUTCONST("-> New mode : ");
    }
    
    /* If the latest mode setting is BLEBeacon. */
    if(pBeaconConfig->mode == ADV_MODE_BLEBEACON)
    {
        SIMPLE_UART_PUTCONST("BLEBeacon");
    }
    /* The latest mode setting is Eddystone. */
    else if(pBeaconConfig->mode == ADV_MODE_EDDYSTONE)
    {
        SIMPLE_UART_PUTCONST("Eddystone");
    }
    /* The latest mode setting is EDTest. */
    else
    {
        SIMPLE_UART_PUTCONST("EDTest");
    }
    SIMPLE_UART_PUTCONST("\r\n");
    
    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_edframe
*******************************************************************************
*
* Summary:
*   Function for getting or changing the edframe flags.
*
* Parameters:
*   None
*  
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_edframe(void)
{
    EDframeFlag frame;
    uint8_t *p;
    const char uid[] = {' ', 'U', 'I', 'D'};
    const char url[] = {' ', 'U', 'R', 'L'};
    const char tlm[] = {' ', 'T', 'L', 'M'};
    
    /* If the command is getting the current edframe setting. */
    if(data_array[sizeof(cmd_EDFRAME)] == '\0')
    {
        SIMPLE_UART_PUTCONST("-> EDFrame mode : ");
    }
    /* If the command is updating the edframe setting. */
    else
    {
        p = data_array + sizeof(cmd_EDFRAME);
        
        strupr((char *)p);
        
        /* mark if the uid\url\tlm option is already there before */  
        frame.UIDisEnabled = false;
        frame.URLisEnabled = false;
        frame.TLMisEnabled = false;
        
        /* If there's still data to be handled. */
        while(*p != '\0')
        {
            /* if the option is " uid" and it's the first time we find it. */
            if((!frame.UIDisEnabled) && (memcmp(p, uid, sizeof(uid)) == 0))
            {
                frame.UIDisEnabled = true;
                p = p + sizeof(uid); 
            }
            /* if the option is " url" and it's the first time we find it. */
            else if((!frame.URLisEnabled) && (memcmp(p, url, sizeof(url)) == 0))
            {
                frame.URLisEnabled = true;
                p = p + sizeof(url); 
            }
            /* if the option is " tlm" and it's the first time we find it. */
            else if((!frame.TLMisEnabled) && (memcmp(p, tlm, sizeof(tlm)) == 0))
            {
                frame.TLMisEnabled = true;
                p = p + sizeof(tlm); 
            }
            /* the command format is wrong */
            else 
            {
                return RESULT_ERROR;
            }
        }
        
        /* "tlm" cannot be enabled alone. 
         * It needs to be enabled together with at least "uid" or "url". */
        if((frame.TLMisEnabled && (!frame.UIDisEnabled) && (!frame.URLisEnabled)) ||
        /* Three frame cannot be enabled at the same time. (For Solar BLE Module) */
            (frame.TLMisEnabled && frame.UIDisEnabled && frame.URLisEnabled))
        {
            return RESULT_ERROR;
        }
    
        /* update the configure */
        ConfigRW_UpdateConfig(CONFIG_TYPE_FRAME, &frame);
        SIMPLE_UART_PUTCONST("-> New EDFrame mode : ");
    }
    
    if(pBeaconConfig->frame.UIDisEnabled)       /* Eddystone-UID is on. */
    {
        SIMPLE_UART_PUTCONST("uid on, ");
    }
    else                                /* Eddystone-UID is off. */
    {
        SIMPLE_UART_PUTCONST("uid off, ");
    }
    
    if(pBeaconConfig->frame.URLisEnabled)       /* Eddystone-URL is on. */
    {
        SIMPLE_UART_PUTCONST("url on, ");
    }
    else                                /* Eddystone-URL is off. */
    {
        SIMPLE_UART_PUTCONST("url off, ");
    }
    
    if(pBeaconConfig->frame.TLMisEnabled)       /* Eddystone-TML is on. */
    {
        SIMPLE_UART_PUTCONST("tlm on");
    }
    else                                /* Eddystone-TML is off. */
    {
        SIMPLE_UART_PUTCONST("tlm off");
    }
    SIMPLE_UART_PUTCONST("\r\n");

    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_lock
*******************************************************************************
*
* Summary:
*   Lock Eddystone-URL Configuration Service and set the single-use lock-code.
*
* Parameters:
*   None
*  
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     The updating operation is rejected in locked state.
******************************************************************************/
static uint16_t cmd_sub_lock(void)
{
    uint8_t tmp_lock[ES_LOCK_LENGTH];
    uint8_t str_buf[ES_LOCK_LENGTH*SIZE_OF_BYTE_TO_02X];
    uint8_t lock_state_locked = true;
    uint8_t *pPos;
    uint8_t i, j;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
    
    /* Check the parameter length. */
    if(strlen((char *)(data_array + sizeof(cmd_EDLOCK))) == SIZE_OF_LOCK_CODE_PARAM + SIZE_OF_PARAM_SPACE)
    {
        pPos = data_array + sizeof(cmd_EDLOCK);
        /* The subsequent character should be a space. */
        if(*pPos++ != ' ')
        {
            return RESULT_ERROR;
        }
        
        /* Copy the parameter to another buffer, check and remove the hyphens. */
        j = 0;
        for(i = 0; i < SIZE_OF_UUID_PARAM; i++)
        {
            /* Don't copy characters in hyphen positions. */
            if((LOCKCODE_STR_HYPHEN_POS_1 == i) || 
               (LOCKCODE_STR_HYPHEN_POS_2 == i) || 
               (LOCKCODE_STR_HYPHEN_POS_3 == i) || 
               (LOCKCODE_STR_HYPHEN_POS_4 == i))
            {
                /* If the character in hyphen positions isn't a hyphen. */
                if(pPos[i] != '-')
                {
                    return RESULT_ERROR;
                }
            }
            /* Copy other characters to the buffer. */
            else
            {
                str_buf[j++] = pPos[i];
            }
        }
        /* Convert and check the parameter. */
        if(hextobin(tmp_lock, str_buf, ES_LOCK_LENGTH) == RESULT_ERROR)
        {
            return RESULT_ERROR;
        }
        
        /* The updating operation is not allowed in locked state. */
        if(pBeaconConfig->LockState)
        {
            return RESULT_REJECTED;
        }
        
        /* Update the lock state and the lock code configuration stored in User SFlash. */
        ConfigRW_UpdateConfig(CONFIG_TYPE_LOCKSTATE, &lock_state_locked);
        ConfigRW_UpdateConfig(CONFIG_TYPE_LOCK, tmp_lock);
        
        /* update Lock State in the GATT database of the GATT Server. */
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE;
        UpdateHandle.value.val  = &(pBeaconConfig->LockState);
        UpdateHandle.value.len  = sizeof(pBeaconConfig->LockState);
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        SIMPLE_UART_PUTCONST("-> New EDSTATE : locked\r\n");
        
        return RESULT_OK;
    }
    /* The parameter length is wrong. */
    else
    {
        return RESULT_ERROR;
    }
}

/******************************************************************************
* Function Name: cmd_sub_unlock
*******************************************************************************
*
* Summary:
*   Unlock Eddystone-URL Configuration Service for modification with the lock-code
*   set by EDLOCK command. User needs to send DEFAULT command if user forgets the 
*   lock-code.
*
* Parameters:
*   None
*  
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
*   RESULT_REJECTED                     Lock-code is not correct.
******************************************************************************/
static uint16_t cmd_sub_unlock(void)
{
    uint8_t tmp_lock[ES_LOCK_LENGTH];
    uint8_t str_buf[ES_LOCK_LENGTH*SIZE_OF_BYTE_TO_02X];
    uint8_t lock_state_unlocked = false;
    uint8_t *pPos;
    uint8_t i, j;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T UpdateHandle;
    
    /* Check the parameter length. */
    if(strlen((char *)(data_array + sizeof(cmd_EDUNLOCK))) == SIZE_OF_LOCK_CODE_PARAM + SIZE_OF_PARAM_SPACE)
    {
        pPos = data_array + sizeof(cmd_EDUNLOCK);
        /* The subsequent character should be a space. */
        if(*pPos++ != ' ')
        {
            return RESULT_ERROR;
        }
        
        /* Copy the parameter to another buffer, check and remove the hyphens. */
        j = 0;
        for(i = 0; i < SIZE_OF_UUID_PARAM; i++)
        {
            /* Don't copy characters in hyphen positions. */
            if((LOCKCODE_STR_HYPHEN_POS_1 == i) || 
               (LOCKCODE_STR_HYPHEN_POS_2 == i) || 
               (LOCKCODE_STR_HYPHEN_POS_3 == i) || 
               (LOCKCODE_STR_HYPHEN_POS_4 == i))
            {
                /* If the character in hyphen positions isn't a hyphen. */
                if(pPos[i] != '-')
                {
                    return RESULT_ERROR;
                }
            }
            /* Copy other characters to the buffer. */
            else
            {
                str_buf[j++] = pPos[i];
            }
        }
        /* Convert and check the parameter. */
        if(hextobin(tmp_lock, str_buf, ES_LOCK_LENGTH) == RESULT_ERROR)
        {
            return RESULT_ERROR;
        }
        
        /* If it's already unlocked, print prompt message. */
        if(!pBeaconConfig->LockState)
        {
            SIMPLE_UART_PUTCONST("-> EDSTATE : unlocked\r\n");
            return RESULT_OK;
        }
        /* It's locked, check whether the code is matched. */
        else if(memcmp(pBeaconConfig->lock, tmp_lock, ES_LOCK_LENGTH) != 0)
        {
            return RESULT_REJECTED;
        }
        
        /* Update the lock state stored in User SFlash. */
        ConfigRW_UpdateConfig(CONFIG_TYPE_LOCKSTATE, &lock_state_unlocked);
        
        /* update Lock State in the GATT database of the GATT Server. */
        UpdateHandle.attrHandle = CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE;
        UpdateHandle.value.val  = &(pBeaconConfig->LockState);
        UpdateHandle.value.len  = sizeof(pBeaconConfig->LockState);
        CyBle_GattsWriteAttributeValue(&UpdateHandle, 
                                       WRITE_ATTRIB_OFFSET_0, 
                                       NULL, 
                                       CYBLE_GATT_DB_LOCALLY_INITIATED);
        
        SIMPLE_UART_PUTCONST("-> New EDSTATE : unlocked\r\n");
        
        return RESULT_OK;
    }
    /* The parameter length is wrong. */
    else
    {
        return RESULT_ERROR;
    }
}

/******************************************************************************
* Function Name: cmd_sub_version
*******************************************************************************
*
* Summary:
*   Function for getting the version.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_version(void)
{
    char firmwareVersion[SIZE_OF_FIRMWARE_INFO_BUF] = {0};

    /* User inputs more than "ver\r\n". */
    if(data_array[sizeof(cmd_VER)] != '\0')
    {
        return RESULT_ERROR;
    }

    /* User input is correct, output the message. */
    GetFirmwareInfo(firmwareVersion);

    SIMPLE_UART_PUTCONST(firmwareVersion);

    return RESULT_OK;
}

/******************************************************************************
* Function Name: cmd_sub_init
*******************************************************************************
*
* Summary:
*   Function for erasing flash and recovering all the configurations.
*
* Parameters:
*   None
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t cmd_sub_init(void)
{
    /* User inputs more than "init\r\n". */
    if(data_array[sizeof(cmd_INIT)] != '\0')
    {
        return RESULT_ERROR;
    } 
    /* User input is correct, set all the settings to default. */
    ConfigRW_WriteDefault();
    /* update GATT database. */
    EddystoneURL_Config_UpdateAllAttrib();
    SIMPLE_UART_PUTCONST("-> Restore to default! \r\n");

    return RESULT_OK;
}

/******************************************************************************
* Function Name: hextobin
*******************************************************************************
*
* Summary:
*   Check and convert Hexadecimal Alphanumeric characters TO Bytes.
*
* Parameters:
*   dest   - The output address where the converted Bytes are stored.
*   src    - Pointer to Hexadecimal Alphanumeric characters to be converted.
*   Nbytes - Count of bytes in dest to be converted.
*
* Return:
*   uint16_t - Return value indicates whether the function succeeded or failed. 
*   Following are the possible error codes.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   RESULT_OK                           No error.
*   RESULT_ERROR                        Incorrect UART input.
******************************************************************************/
static uint16_t hextobin(uint8_t *dest, const uint8_t *src, uint8_t Nbytes)
{
    const uint8_t *p;
    uint32_t i;
    uint16_t tmp_result;
    
    /* Check the input parameters. */
    if((dest == NULL) || (src == NULL))
    {
        return RESULT_ERROR;
    }
    
    p = src;
    
    for(i = 0; i < Nbytes; i++, p += SIZE_OF_BYTE_TO_02X)
    {
        tmp_result = twoaton(p);
        /* If the subsequent two characters are not in the range of '0'-'9', 'a'-'f'/'A'-'F'. */
        if(tmp_result == TWOATON_PARAM_ERROR)
        {
            return RESULT_ERROR;
        }
        dest[i] = tmp_result;
    }
    
    return RESULT_OK;
}

/******************************************************************************
* Function Name: twoaton
*******************************************************************************
*
* Summary:
*   Function for converting two characters in hex format to a number.
*   For example:    "31" to 0x31(49) and "41" to 0x41(65).
*
* Parameters:
*   p - Pointer to the two characters to be converted.
*
* Return:
*   uint16_t - The converted output, it is from 0x00 to 0xFF. 
*              If the input parameter is wrong, 0xFFFF is returned.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   0x0000 - 0x00FF                     No error.
*   CTON_PARAM_ERROR                    Incorrect parameter.
******************************************************************************/
static uint16_t twoaton(const uint8_t *p)
{
    uint8_t upper_nibble;
    uint8_t lower_nibble;
    
    upper_nibble = atox(*p++);
    lower_nibble = atox(*p);
    /* If the character is not in '0' and 'f/F'. */
    if((upper_nibble == ATOX_PARAM_ERROR) || 
       (lower_nibble == ATOX_PARAM_ERROR))
    {
        return TWOATON_PARAM_ERROR;
    }
    return (uint16_t)((upper_nibble << NIBBLE_BITS) | lower_nibble);
}

/******************************************************************************
* Function Name: atox
*******************************************************************************
*
* Summary:
*   Function for converting ASCII to number.
*
* Parameters:
*   data - The ASCII byte to be converted.
*
* Return:
*   uint8_t - The converted output, it is from 0x00 to 0x0F. 
*             If the input parameter is wrong, 0xFF is returned.
*   
*   Errors codes                        Description
*   ------------                        -----------
*   0x00 - 0x0F                         No error.
*   ATOX_PARAM_ERROR                    Incorrect parameter.
******************************************************************************/
static uint8_t atox(uint8_t data)
{
    if((data >= '0') && (data <= '9'))
    {
        /* 0..9 */
        return (data - '0');
    }
    else if((data >= 'A') && (data <= 'F'))
    {
        /* A..F */
        return (data - 'A' + 0x0A);
    }
    else if((data >= 'a') && (data <= 'f'))
    {
        /* a..f */
        return (data - 'a' + 0x0A);
    }
    else
    {
        return ATOX_PARAM_ERROR;    /* return error value. */
    }
}

/* [] END OF FILE */
