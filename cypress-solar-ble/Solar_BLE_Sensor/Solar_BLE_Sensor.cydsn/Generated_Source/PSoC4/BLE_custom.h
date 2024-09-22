/***************************************************************************//**
* \file CYBLE_custom.h
* \version 3.30
* 
* \brief
*  Contains the function prototypes and constants for the Custom Service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_CUSTOM_H)
#define CY_BLE_CYBLE_CUSTOM_H

#include "BLE_gatt.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Maximum supported Custom Services */
#define CYBLE_CUSTOMS_SERVICE_COUNT                  (0x01u)
#define CYBLE_CUSTOMC_SERVICE_COUNT                  (0x00u)
#define CYBLE_CUSTOM_SERVICE_CHAR_COUNT              (0x09u)
#define CYBLE_CUSTOM_SERVICE_CHAR_DESCRIPTORS_COUNT  (0x00u)

/* Below are the indexes and handles of the defined Custom Services and their characteristics */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_SERVICE_INDEX   (0x00u) /* Index of Eddystone-URL Configuration service in the cyBle_customs array */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_INDEX   (0x00u) /* Index of Lock State characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_CHAR_INDEX   (0x01u) /* Index of Lock characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_UNLOCK_CHAR_INDEX   (0x02u) /* Index of Unlock characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_INDEX   (0x03u) /* Index of URI Data characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_INDEX   (0x04u) /* Index of URI Flags characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_INDEX   (0x05u) /* Index of Advertised Tx Power Levels characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_INDEX   (0x06u) /* Index of Tx Power Mode characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_INDEX   (0x07u) /* Index of Beacon Period characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_RESET_CHAR_INDEX   (0x08u) /* Index of Reset characteristic */


#define CYBLE_EDDYSTONEURL_CONFIGURATION_SERVICE_HANDLE   (0x000Cu) /* Handle of Eddystone-URL Configuration service */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_DECL_HANDLE   (0x000Du) /* Handle of Lock State characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_STATE_CHAR_HANDLE   (0x000Eu) /* Handle of Lock State characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_DECL_HANDLE   (0x000Fu) /* Handle of Lock characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_LOCK_CHAR_HANDLE   (0x0010u) /* Handle of Lock characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_UNLOCK_DECL_HANDLE   (0x0011u) /* Handle of Unlock characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_UNLOCK_CHAR_HANDLE   (0x0012u) /* Handle of Unlock characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_DECL_HANDLE   (0x0013u) /* Handle of URI Data characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_URI_DATA_CHAR_HANDLE   (0x0014u) /* Handle of URI Data characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_DECL_HANDLE   (0x0015u) /* Handle of URI Flags characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_URI_FLAGS_CHAR_HANDLE   (0x0016u) /* Handle of URI Flags characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_DECL_HANDLE   (0x0017u) /* Handle of Advertised Tx Power Levels characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_ADVERTISED_TX_POWER_LEVELS_CHAR_HANDLE   (0x0018u) /* Handle of Advertised Tx Power Levels characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_DECL_HANDLE   (0x0019u) /* Handle of Tx Power Mode characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_TX_POWER_MODE_CHAR_HANDLE   (0x001Au) /* Handle of Tx Power Mode characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_DECL_HANDLE   (0x001Bu) /* Handle of Beacon Period characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_BEACON_PERIOD_CHAR_HANDLE   (0x001Cu) /* Handle of Beacon Period characteristic */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_RESET_DECL_HANDLE   (0x001Du) /* Handle of Reset characteristic declaration */
#define CYBLE_EDDYSTONEURL_CONFIGURATION_RESET_CHAR_HANDLE   (0x001Eu) /* Handle of Reset characteristic */



#if(CYBLE_CUSTOMS_SERVICE_COUNT != 0u)
    #define CYBLE_CUSTOM_SERVER
#endif /* (CYBLE_CUSTOMS_SERVICE_COUNT != 0u) */
    
#if(CYBLE_CUSTOMC_SERVICE_COUNT != 0u)
    #define CYBLE_CUSTOM_CLIENT
#endif /* (CYBLE_CUSTOMC_SERVICE_COUNT != 0u) */

/***************************************
* Data Struct Definition
***************************************/

/**
 \addtogroup group_service_api_custom
 @{
*/

#ifdef CYBLE_CUSTOM_SERVER

/** Contains information about Custom Characteristic structure */
typedef struct
{
    /** Custom Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharHandle;
    /** Custom Characteristic Descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharDesc[     /* MDK doesn't allow array with zero length */
        CYBLE_CUSTOM_SERVICE_CHAR_DESCRIPTORS_COUNT == 0u ? 1u : CYBLE_CUSTOM_SERVICE_CHAR_DESCRIPTORS_COUNT];
} CYBLE_CUSTOMS_INFO_T;

/** Structure with Custom Service attribute handles. */
typedef struct
{
    /** Handle of a Custom Service */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServHandle;
    
    /** Information about Custom Characteristics */
    CYBLE_CUSTOMS_INFO_T customServInfo[                /* MDK doesn't allow array with zero length */
        CYBLE_CUSTOM_SERVICE_CHAR_COUNT == 0u ? 1u : CYBLE_CUSTOM_SERVICE_CHAR_COUNT];
} CYBLE_CUSTOMS_T;


#endif /* (CYBLE_CUSTOM_SERVER) */

/** @} */

/** \cond IGNORE */
/* The custom Client functionality is not functional in current version of 
* the component.
*/
#ifdef CYBLE_CUSTOM_CLIENT

typedef struct
{
    /** Custom Descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T descHandle;
	/** Custom Descriptor 128 bit UUID */
	const void *uuid;           
    /** UUID Format - 16-bit (0x01) or 128-bit (0x02) */
	uint8 uuidFormat;
   
} CYBLE_CUSTOMC_DESC_T;

typedef struct
{
    /** Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharHandle;
	/** Characteristic end handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharEndHandle;
	/** Custom Characteristic UUID */
	const void *uuid;           
    /** UUID Format - 16-bit (0x01) or 128-bit (0x02) */
	uint8 uuidFormat;
    /** Properties for value field */
    uint8  properties;
	/** Number of descriptors */
    uint8 descCount;
    /** Characteristic Descriptors */
    CYBLE_CUSTOMC_DESC_T * customServCharDesc;
} CYBLE_CUSTOMC_CHAR_T;

/** Structure with discovered attributes information of Custom Service */
typedef struct
{
    /** Custom Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServHandle;
	/** Custom Service UUID */
	const void *uuid;           
    /** UUID Format - 16-bit (0x01) or 128-bit (0x02) */
	uint8 uuidFormat;
	/** Number of characteristics */
    uint8 charCount;
    /** Custom Service Characteristics */
    CYBLE_CUSTOMC_CHAR_T * customServChar;
} CYBLE_CUSTOMC_T;

#endif /* (CYBLE_CUSTOM_CLIENT) */
/** \endcond */

#ifdef CYBLE_CUSTOM_SERVER

extern const CYBLE_CUSTOMS_T cyBle_customs[CYBLE_CUSTOMS_SERVICE_COUNT];

#endif /* (CYBLE_CUSTOM_SERVER) */

/** \cond IGNORE */
#ifdef CYBLE_CUSTOM_CLIENT

extern CYBLE_CUSTOMC_T cyBle_customc[CYBLE_CUSTOMC_SERVICE_COUNT];

#endif /* (CYBLE_CUSTOM_CLIENT) */
/** \endcond */


/***************************************
* Private Function Prototypes
***************************************/

/** \cond IGNORE */
void CyBle_CustomInit(void);

#ifdef CYBLE_CUSTOM_CLIENT

void CyBle_CustomcDiscoverServiceEventHandler(const CYBLE_DISC_SRVC128_INFO_T *discServInfo);
void CyBle_CustomcDiscoverCharacteristicsEventHandler(uint16 discoveryService, const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
CYBLE_GATT_ATTR_HANDLE_RANGE_T CyBle_CustomcGetCharRange(uint8 incrementIndex);
void CyBle_CustomcDiscoverCharDescriptorsEventHandler(const CYBLE_DISC_DESCR_INFO_T *discDescrInfo);

#endif /* (CYBLE_CUSTOM_CLIENT) */

/** \endcond */

/***************************************
* External data references 
***************************************/

#ifdef CYBLE_CUSTOM_CLIENT

extern CYBLE_CUSTOMC_T cyBle_customCServ[CYBLE_CUSTOMC_SERVICE_COUNT];

#endif /* (CYBLE_CUSTOM_CLIENT) */


/** \cond IGNORE */
/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/
#define customServiceCharHandle         customServCharHandle
#define customServiceCharDescriptors    customServCharDesc
#define customServiceHandle             customServHandle
#define customServiceInfo               customServInfo
/** \endcond */


#endif /* CY_BLE_CYBLE_CUSTOM_H  */

/* [] END OF FILE */
