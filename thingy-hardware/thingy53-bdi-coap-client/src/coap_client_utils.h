/**
 * coap_client_utils.h
 */

#ifndef __COAP_CLIENT_UTILS_H__
#define __COAP_CLIENT_UTILS_H__

#include <coap_server_client_interface.h>

/** @brief Type indicates function called when OpenThread connection
 *         is established.
 *
 * @param[in] item pointer to work item.
 */
typedef void (*ot_connection_cb_t)(struct k_work *item);

/** @brief Type indicates function called when OpenThread connection is ended.
 *
 * @param[in] item pointer to work item.
 */
typedef void (*ot_disconnection_cb_t)(struct k_work *item);

/** @brief Type indicates function called when the MTD modes are toggled.
 *
 * @param[in] val 1 if the MTD is in MED mode
 *                0 if the MTD is in SED mode
 */
typedef void (*mtd_mode_toggle_cb_t)(uint32_t val);

typedef void (*get_org_spec_xml_cb_t)(uint8_t *data, uint16_t size, uint8_t xml_part, uint8_t total_xml_parts);

typedef void (*post_role_cb_t)(bool created);

typedef void (*delete_role_cb_t)(bool success);

typedef void (*get_reward_cb_t)(int reward, const char *role_id, bool role_player);


/** @brief Initialize CoAP client utilities.
 */
int coap_client_utils_init(ot_connection_cb_t on_connect,
			    ot_disconnection_cb_t on_disconnect,
			    mtd_mode_toggle_cb_t on_toggle,
				get_org_spec_xml_cb_t on_get_xml,
				post_role_cb_t on_post_role,
				delete_role_cb_t on_delete_role,
				get_reward_cb_t on_get_reward);

/**************************************************************************************************
  Initializes GET organizational specification XML request to CoAP server.
**************************************************************************************************/
void coap_client_get_xml(void);
/**************************************************************************************************
  Initializes GET organizational specification XML request to CoAP server.
**************************************************************************************************/
void coap_client_get_xml_segments(const char *uri, const char *query);

/**************************************************************************************************
  Initializes GET BME688 server data request.
**************************************************************************************************/
void coap_client_get_bme688(uint8_t agent_id, uint8_t sensor);

/**************************************************************************************************
  Initializes PUT BME688 server data buffer request.
**************************************************************************************************/
void coap_client_put_bme688(const char *uri, const char *bme688_data);

void coap_client_post_role(const char *uri, const char *agent_id_str, size_t agent_id_len);

void coap_client_delete_role(const char *uri);

void coap_client_get_reward(const char *uri);

/** @brief PUT BME688 sensor data buffer on the CoAP server.
 */
void submit_coap_request(const char *uri, uint8_t coap_code, uint8_t coap_type,
								 uint32_t observe, const char *payload, size_t payload_size);

/** @brief Toggle SED to MED and MED to SED modes.
 *
 * @note Active when the device is working as Minimal Thread Device.
 */
void coap_client_toggle_minimal_sleepy_end_device(void);

#endif

/**
 * @}
 */
