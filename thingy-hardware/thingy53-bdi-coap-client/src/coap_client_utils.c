/**
 * coap_client_utils.c
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <zephyr/drivers/gpio.h>
#include <dk_buttons_and_leds.h>

#include "coap_client_utils.h"
#include "peripherals/gpio.h"

#define RED_LED				DK_LED1
#define GREEN_LED			DK_LED2
#define BLUE_LED			DK_LED3


LOG_MODULE_REGISTER(coap_client_utils, CONFIG_COAP_CLIENT_UTILS_LOG_LEVEL);

/**************************************************************************************************
  Indicates if connected to CoAP network.
**************************************************************************************************/
static bool is_connected;

/**************************************************************************************************
  CoAP request parameters.
**************************************************************************************************/
otCoapCode requestCode = OT_COAP_CODE_EMPTY;
otCoapType requestType = OT_COAP_TYPE_CONFIRMABLE;
uint32_t requestObserver = 0;

/**************************************************************************************************
  Workqueue items for CoAP transmission
**************************************************************************************************/
static struct k_work toggle_MTD_SED_work;
static struct k_work on_connect_work;
static struct k_work on_disconnect_work;
static struct k_work get_xml_segment_work;
static struct k_work bme688_work;
static struct k_work post_role_work;
static struct k_work cancel_observe_work;
static struct k_work delete_role_work;
static struct k_work get_reward_work;
static struct k_work coap_request_work;

/**************************************************************************************************
  Callback functions for main.cpp
**************************************************************************************************/
mtd_mode_toggle_cb_t on_mtd_mode_toggle;
get_org_spec_xml_cb_t on_get_org_spec_xml;
post_role_cb_t on_post_role_request;
delete_role_cb_t on_delete_role_request;
get_reward_cb_t on_get_reward_request;

/**************************************************************************************************
  IPv6 Address of CoAP Server
**************************************************************************************************/
static const char *const serverAddr = "fd19:99a6:cda:f9e6:0:ff:fe00:fc10";

/**************************************************************************************************
  Buffer and Size of payload of outgoing CoAP message
**************************************************************************************************/
static uint8_t payload_nnn[1024];
static uint8_t *payload_ptr = payload_nnn;
static uint16_t payload_size;

/**************************************************************************************************
  Buffer for data of received CoAP message
**************************************************************************************************/
static uint8_t received_data[1024];
static uint8_t *data_ptr = received_data;

/**************************************************************************************************
  Buffer for URI Path of outgoing CoAP message
**************************************************************************************************/
static char uriPathBuffer[180];
static char *uriPath = uriPathBuffer;

/**************************************************************************************************
  Buffer for Query Option of outgoing CoAP message
**************************************************************************************************/
static char queryBuffer[32];
static char *queryOption = queryBuffer;



static void default_response_handler(void *p_context,
							 otMessage *p_message,
							 const otMessageInfo *p_message_info,
							 otError result)
{
	gpio_pin_set_dt(&out0, 0);
	uint16_t received_size;

	LOG_INF("Default Response Handler, code:%s\n:", otCoapMessageCodeToString(p_message));

	uint16_t length = otMessageGetLength(p_message) - otMessageGetOffset(p_message);
	
	if (result == OT_ERROR_NONE) {
		LOG_INF("Received ACK!\n");
	}
	else {
		LOG_WRN("Delivery not confirmed!\n");
		goto exit;
	}
	
	received_size = otMessageRead(p_message, otMessageGetOffset(p_message), 
							data_ptr, length);
	
	data_ptr[length] = '\0';
	
	if (data_ptr == NULL) {
		LOG_ERR("Received data is invalid");
		goto exit;
	}
	LOG_INF("Size of Received: %i \n", length);
	LOG_INF("Data: %s \n", data_ptr);

exit:
	return;
}

static void on_get_xml_segment_reply(void *p_context,
							 otMessage *p_message,
							 const otMessageInfo *p_message_info,
							 otError result)
{
	gpio_pin_set_dt(&out0, 0); //GPIO
	uint8_t received_xml_part = 0;
	uint8_t total_xml_parts = 0;

	if (result == OT_ERROR_NONE) {
		LOG_INF("%i: Received XML successfully.\n", log_iterator++);
	}
	else {
		LOG_ERR("Error occured while getting XML.\n");
		goto exit;
	}

	uint16_t length = otMessageGetLength(p_message) - otMessageGetOffset(p_message);
	
	uint16_t received_size = otMessageRead(p_message, otMessageGetOffset(p_message), 
								data_ptr, length);

	if (data_ptr == NULL) {
		LOG_ERR("Received xml data is invalid");
		goto exit;
	}
	memcpy(&received_xml_part, data_ptr, 1);
	memcpy(&total_xml_parts, data_ptr+1, 1);
	on_get_org_spec_xml(data_ptr+2, received_size-2, received_xml_part, total_xml_parts);

exit:
	return;
}

static void on_put_bme688_reply(void *p_context,
								otMessage *p_message,
								const otMessageInfo *p_message_info,
								otError result){

	gpio_pin_set_dt(&out0, 0); //GPIO
	if (result == OT_ERROR_NONE) {
		LOG_INF("%i: Delivery of BME688 sensor data confirmed.\n", log_iterator++);
		goto exit;
	}
	else {
		LOG_WRN("Delivery of BME688 sensor data not confirmed.\n");
		goto exit;
	}

exit:
	return;
}

static void on_post_role_reply(void *p_context,
							   otMessage *p_message,
							   const otMessageInfo *p_message_info,
							   otError result){

	gpio_pin_set_dt(&out0, 0);
	otCoapCode code;
	otCoapType type;

	if (result == OT_ERROR_NONE) {
		LOG_INF("%i: Received POST ROLE response successfully.\n", log_iterator++);
		dk_set_led(GREEN_LED, 1);
		dk_set_led(RED_LED, 1);
	}
	else {
		LOG_WRN("Error occured during POST ROLE request.\n");
		goto exit;
	}

	type = otCoapMessageGetType(p_message);
	if (type != OT_COAP_TYPE_ACKNOWLEDGMENT) {
		LOG_WRN("Received unexpected CoAP code.\n");
		goto exit;
	}

	code = otCoapMessageGetCode(p_message);
	LOG_INF("With Code:%s\n:", otCoapMessageCodeToString(p_message));

	if (code == OT_COAP_CODE_CREATED) {
		on_post_role_request(true);
	}
	else {
		on_post_role_request(false);
	}
	
exit:

	return;
}

static void on_delete_role_reply(void *p_context,
							   otMessage *p_message,
							   const otMessageInfo *p_message_info,
							   otError result){
	gpio_pin_set_dt(&out0, 0);
	otCoapCode code;
	otCoapType type;

	if (result == OT_ERROR_NONE) {
		LOG_INF("%i: Received DELETE ROLE response successfully.\n", log_iterator++);
	}
	else {
		LOG_ERR("Error occured during DELETE ROLE request.\n");
		goto exit;
	}

	type = otCoapMessageGetType(p_message);
	code = otCoapMessageGetCode(p_message);
	if (type != OT_COAP_TYPE_ACKNOWLEDGMENT) {
		LOG_WRN("Received unexpected CoAP type.\n");
		goto exit;
	}
	LOG_INF("With code: %s\n",  otCoapMessageCodeToString(p_message));
	if (code != OT_COAP_CODE_DELETED) {
		LOG_WRN("Received unexpected CoAP code.\n");
		goto exit;
	}

	on_delete_role_request(true);

exit:
	return;
}

static void on_cancel_observe_reply(void *p_context,
							   otMessage *p_message,
							   const otMessageInfo *p_message_info,
							   otError result){
	gpio_pin_set_dt(&out0, 0);
	otCoapCode code;
	otCoapType type;

	if (result == OT_ERROR_NONE) {
		LOG_INF("Received CANCEL OBSERVE ACK successfully.\n");
	}
	else {
		LOG_ERR("Error occured during CANCEL OBSERVE request.\n");
		goto exit;
	}

	type = otCoapMessageGetType(p_message);
	code = otCoapMessageGetCode(p_message);
	if (type != OT_COAP_TYPE_ACKNOWLEDGMENT) {
		LOG_WRN("Received unexpected CoAP type.\n");
		goto exit;
	}
	LOG_INF("With code: %s\n",  otCoapMessageCodeToString(p_message));
	if (code != OT_COAP_CODE_CONTENT) {
		LOG_WRN("Received unexpected CoAP code.\n");
		goto exit;
	}

exit:
	return;

}
static void on_get_reward_reply(void *p_context,
							   otMessage *p_message,
							   const otMessageInfo *p_message_info,
							   otError result){

	gpio_pin_set_dt(&out0, 0);
	int reward = 0;
	uint16_t received_size;
	char role[40];
	char agent[10];

	if (result == OT_ERROR_NONE) {
		LOG_INF("%i: Received GET REWARD successfully.\n", log_iterator++);
	}
	else {
		LOG_WRN("Delivery not confirmed!\n");
		goto exit;
	}

	uint16_t length = otMessageGetLength(p_message) - otMessageGetOffset(p_message);

	otCoapType type = otCoapMessageGetType(p_message);
	LOG_INF("With code: %s\n",  otCoapMessageCodeToString(p_message));
	if (type != OT_COAP_TYPE_ACKNOWLEDGMENT &&
		type != OT_COAP_TYPE_CONFIRMABLE) {
		LOG_WRN("Received unexpected CoAP type.\n");
		goto exit;
	}

	otCoapCode code = otCoapMessageGetCode(p_message);
	if (code != OT_COAP_CODE_CONTENT) {
		LOG_WRN("Received unexpected CoAP code.\n");
		goto exit;
	}

	received_size = otMessageRead(p_message, otMessageGetOffset(p_message), 
									received_data, length);
	received_data[length] = '\0';

	if (received_data == NULL) {
		LOG_ERR("Received reward data is invalid");
		goto exit;
	}
	int conversions = sscanf(received_data, "reward:%d;rid:%[^;];aid:%s", &reward, role, agent);

	if(conversions == 3){
        on_get_reward_request(reward, &role[0],1);
	} else if (conversions == 2) {
		on_get_reward_request(reward, &role[0],0);
	} else {
		LOG_WRN("Received invalid rewards string!");
	}

exit:
	return;
}

/**************************************************************************************************
  Generic function to initialize CoAP message.
**************************************************************************************************/
otError init_coap_message(otMessage *aMessage, char *aUriPath, char *aQuery, otCoapType aType, 
						  otCoapCode aCode, uint32_t aObserve)
{	
	
	otError error = OT_ERROR_NONE;
	
	otCoapMessageInit(aMessage, aType, aCode);

	if (aObserve != 1){
		otCoapMessageGenerateToken(aMessage, OT_COAP_DEFAULT_TOKEN_LENGTH);
		error = otCoapMessageAppendObserveOption(aMessage, aObserve);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Append Observe Option Error\n");
			return error;
		}
	}
		
	error = otCoapMessageAppendUriPathOptions(aMessage, aUriPath);
	if (error != OT_ERROR_NONE){ 
		LOG_ERR("URI PATH FAIL\n");
		return error;
	}

	error = otCoapMessageAppendContentFormatOption(aMessage, 
										OT_COAP_OPTION_CONTENT_FORMAT_TEXT_PLAIN);
	if (error != OT_ERROR_NONE){ 
		LOG_ERR("Content Error\n");
		return error;
	}

	if (strlen(aQuery) != 0){
		error = otCoapMessageAppendUriQueryOption(aMessage, aQuery);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("QUERY OPTION FAIL\n");
			return error;
		}
	}
	
	error = otCoapMessageSetPayloadMarker(aMessage);
	if (error != OT_ERROR_NONE){ 
		LOG_ERR("Set Payload Marker Error\n");
		return error;
	}
	return error;
}

static void get_xml_segment_request(struct k_work *item) {	
	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();
	
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			return;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, OT_COAP_TYPE_CONFIRMABLE, 
									OT_COAP_CODE_GET, 0);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1); //GPIO
		gpio_pin_set_dt(&out1, 1);
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
		 							 on_get_xml_segment_reply, NULL);
		gpio_pin_set_dt(&out1, 0);
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to send GET ORG SPEC XML request\n");
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("%i: GET ORG SPEC XML sent success\n", log_iterator++);	
	}
}

/**************************************************************************************************
  Sends PUT request with BME688 sensor data to the CoAP server.
**************************************************************************************************/
static void bme688_request(struct k_work *item)
{	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();

	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, requestType, requestCode, requestObserver);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		error = otMessageAppend(myMessage, payload_ptr, payload_size);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Append Payload Error\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1); //GPIO
		gpio_pin_set_dt(&out1, 1); //GPIO
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
										on_put_bme688_reply, NULL);
		gpio_pin_set_dt(&out1, 0); //GPIO
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to send BME688 request: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("%i: BME688 sensor data sent successfully\n", log_iterator++);
	}
	return;
}

/**************************************************************************************************
  Sends POST ROLE request to the CoAP server.
**************************************************************************************************/
static void post_role_request(struct k_work *item)
{	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();
	
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, requestType, requestCode, requestObserver);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		error = otMessageAppend(myMessage, payload_ptr, payload_size);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Append Payload Error\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1);
		gpio_pin_set_dt(&out1, 1); //GPIO
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
										on_post_role_reply, NULL);
		gpio_pin_set_dt(&out1, 0); //GPIO
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to send POST ROLE request: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("%i: POST ROLE sent success\n", log_iterator++);
	}
	return;
}

/**************************************************************************************************
  Sends DELETE ROLE request to the CoAP server.
**************************************************************************************************/
static void delete_role_request(struct k_work *item)
{	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();
	
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, OT_COAP_TYPE_CONFIRMABLE, 
										OT_COAP_CODE_DELETE, requestObserver);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1);
		gpio_pin_set_dt(&out1, 1); //GPIO
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
										on_delete_role_reply, NULL);
		gpio_pin_set_dt(&out1, 0); //GPIO
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to send DELETE ROLE request: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("%i: DELETE ROLE sent success\n", log_iterator++);
	}
	return;
}

/**************************************************************************************************
  Sends GET request without observe to the CoAP server.
**************************************************************************************************/
static void cancel_observe_request(struct k_work *item)
{	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();
	
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, OT_COAP_TYPE_CONFIRMABLE, 
										OT_COAP_CODE_GET, requestObserver);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1);
		gpio_pin_set_dt(&out1, 1); //GPIO
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
										on_cancel_observe_reply, NULL);
		gpio_pin_set_dt(&out1, 0); //GPIO
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to send CANCEL OBSERVE request: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("%i: CANCEL OBSERVE sent successfully\n", log_iterator++);
	}
	return;
}

static void get_reward_request(struct k_work *item)
{	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();
	
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, requestType, requestCode, requestObserver);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1);
		gpio_pin_set_dt(&out1, 1); //GPIO
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
										on_get_reward_reply, NULL);
		gpio_pin_set_dt(&out1, 0); //GPIO
		
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_WRN("Failed to send GET ROLE request: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("%i: GET REWARD sent success\n", log_iterator++);
	}
	return;
}

static void do_work_coap_request(struct k_work *item)
{	
	ARG_UNUSED(item);

	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();
	
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			LOG_ERR("Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddr, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, requestType, requestCode, requestObserver);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("CoAP message initialization error!\n");
			break;
		}
		error = otMessageAppend(myMessage, payload_ptr, payload_size);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("Append Payload Error\n");
			break;
		}
		gpio_pin_set_dt(&out0, 1);
		gpio_pin_set_dt(&out1, 1); //GPIO
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
									default_response_handler, NULL);
		gpio_pin_set_dt(&out1, 0); //GPIO
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to send generic request with code: %s\n", otCoapMessageCodeToString(myMessage));
		LOG_ERR("Error Code: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		LOG_INF("Successfully sent generic request with code: %s\n", otCoapMessageCodeToString(myMessage));
	}
	return;
}


/**************************************************************************************************
  Default handler for incoming CoAP messages.
**************************************************************************************************/
static void coap_default_handler(void *p_context,
								otMessage *p_message,
								const otMessageInfo *p_message_info)
{
	ARG_UNUSED(p_message_info);
	ARG_UNUSED(p_message);
	ARG_UNUSED(p_context);

	LOG_WRN("Received CoAP message that does not match any resource!\n");

}

/**************************************************************************************************
  Switches from minimal (MED) to sleepy end device (SED) and vice versa.
**************************************************************************************************/
static void toggle_minimal_sleepy_end_device(struct k_work *item)
{
	otError error;
	otLinkModeConfig mode;
	struct openthread_context *context = openthread_get_default_context();

	__ASSERT_NO_MSG(context != NULL);

	openthread_api_mutex_lock(context);
	mode = otThreadGetLinkMode(context->instance);
	mode.mRxOnWhenIdle = !mode.mRxOnWhenIdle;
	error = otThreadSetLinkMode(context->instance, mode);
	openthread_api_mutex_unlock(context);

	if (error != OT_ERROR_NONE) {
		LOG_ERR("Failed to set MLE link mode configuration");
	} else {
		on_mtd_mode_toggle(mode.mRxOnWhenIdle);
	}
}

static void update_device_state(void)
{
	struct otInstance *instance = openthread_get_default_instance();
	otLinkModeConfig mode = otThreadGetLinkMode(instance);
	on_mtd_mode_toggle(mode.mRxOnWhenIdle);
}

static void on_thread_state_changed(uint32_t flags, void *context)
{
	struct openthread_context *ot_context = context;

	if (flags & OT_CHANGED_THREAD_ROLE) {
		switch (otThreadGetDeviceRole(ot_context->instance)) {
		case OT_DEVICE_ROLE_CHILD:
		case OT_DEVICE_ROLE_ROUTER:
		case OT_DEVICE_ROLE_LEADER:
			k_work_submit(&on_connect_work);
			is_connected = true;
			break;

		case OT_DEVICE_ROLE_DISABLED:
		case OT_DEVICE_ROLE_DETACHED:
		default:
			k_work_submit(&on_disconnect_work);
			is_connected = false;
			break;
		}
	}
}

/**************************************************************************************************
  Submits CoAP work item if device is connected to Thread.
**************************************************************************************************/
static void submit_work_if_connected(struct k_work *work)
{
	if (is_connected) {
		k_work_submit(work);
	} else {
		LOG_WRN("Connection is broken");
	}
}


/**************************************************************************************************
  Initializes CoAP functionality.
**************************************************************************************************/
int coap_client_utils_init(ot_connection_cb_t on_connect,
			    ot_disconnection_cb_t on_disconnect,
			    mtd_mode_toggle_cb_t on_toggle,
				get_org_spec_xml_cb_t on_get_xml,
				post_role_cb_t on_post_role,
				delete_role_cb_t on_delete_role,
				get_reward_cb_t on_get_reward)
{
	otError error;

	/* Callback functions to main */
	on_mtd_mode_toggle = on_toggle;
	on_get_org_spec_xml = on_get_xml;
	on_post_role_request = on_post_role;
	on_delete_role_request = on_delete_role;
	on_get_reward_request = on_get_reward;

	otInstance *p_instance = openthread_get_default_instance();
	/* Create CoAP instnce and start CoAP */
	error = otCoapStart(p_instance, COAP_PORT);
	if (error!=OT_ERROR_NONE){
		LOG_ERR("Failed to start Coap: %d\n", error);
		goto end;
	}

	/* Assign CoAP default handler */
	otCoapSetDefaultHandler(p_instance, coap_default_handler, NULL);

	/* Initialize CoAP work structure with handlers */
	k_work_init(&on_connect_work, on_connect);
	k_work_init(&on_disconnect_work, on_disconnect);
	k_work_init(&get_xml_segment_work, get_xml_segment_request);
	k_work_init(&bme688_work, bme688_request);
	k_work_init(&post_role_work, post_role_request);
	k_work_init(&delete_role_work, delete_role_request); 
	k_work_init(&cancel_observe_work, cancel_observe_request);
	k_work_init(&get_reward_work, get_reward_request);
	k_work_init(&coap_request_work, do_work_coap_request);

	/* Start OpenThread */
	openthread_set_state_changed_cb(on_thread_state_changed);
	openthread_start(openthread_get_default_context());

	/* Toggle MED and SED if enabled */
	if (IS_ENABLED(CONFIG_OPENTHREAD_MTD_SED)) {
		k_work_init(&toggle_MTD_SED_work,
			    toggle_minimal_sleepy_end_device);
		update_device_state();
	}

end:
	return error == OT_ERROR_NONE ? 0 : 1;
}


/**************************************************************************************************
  Extern functions.
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Initializes CoAP request to server with passed parameters.
 *
 *  \param  uri: 			URI Path to server resource.
 *  \param  coap_code: 		CoAP code (e.g. PUT, POST, GET).
 *  \param  coap_type: 		Confirmable or non-confirmable.
 *  \param  observe: 		0 if resource should be observed.
 *  \param  payload: 		Char pointer to payload string.
 * 	\param  payload_size: 	Size of payload in bytes.
 * 
 *  \return None.
 */
/*************************************************************************************************/
void submit_coap_request(const char *uri, uint8_t coap_code, uint8_t coap_type,
								 uint32_t observe, const char *payload, size_t size)
{
	strncpy(uriPath, uri, 150);
	strncpy(queryOption, "", 32);
	requestCode = coap_code;
	requestType = coap_type;
	requestObserver = observe;
	memcpy(payload_ptr, payload, size);
	payload_size = size;
	submit_work_if_connected(&coap_request_work);
}

/**************************************************************************************************
  Initializes GET organizational specification XML request to CoAP server.
**************************************************************************************************/
void coap_client_get_xml_segments(const char *uri, const char *query)
{
	strncpy(uriPath, uri, 150);
	strncpy(queryOption, query, 32);
	submit_work_if_connected(&get_xml_segment_work);
}


/**************************************************************************************************
  Initializes PUT BME688 server data buffer request.
**************************************************************************************************/
void coap_client_put_bme688(const char *uri, const char *bme688_data)
{	
	strncpy(uriPath, uri, 150);
	strncpy(queryOption, "", 32);
	requestCode = OT_COAP_CODE_PUT;
	requestType = OT_COAP_TYPE_CONFIRMABLE;
	requestObserver = 0; // Do not observe
	memcpy(payload_ptr, bme688_data, strlen(bme688_data));
	payload_size = strlen(bme688_data);
	submit_work_if_connected(&bme688_work);
}

void coap_client_get_reward(const char *uri)
{
	strncpy(uriPath, uri, 180);
	strncpy(queryOption, "", 32);
	requestCode = OT_COAP_CODE_GET;
	requestType = OT_COAP_TYPE_CONFIRMABLE;
	requestObserver = 0; // Do observe
	submit_work_if_connected(&get_reward_work);
}

void coap_client_post_role(const char *uri, const char *agent_id_str, size_t agent_id_len)
{
	strncpy(uriPath, uri, 150);
	strncpy(queryOption, "", 32);
	requestCode = OT_COAP_CODE_POST;
	requestType = OT_COAP_TYPE_CONFIRMABLE;
	requestObserver = 0; // Do observe
	memcpy(payload_ptr, agent_id_str, agent_id_len);
	payload_size = agent_id_len;
	submit_work_if_connected(&post_role_work);
}

void coap_client_delete_role(const char *uri)
{
	strncpy(uriPath, uri, 180);
	strncpy(queryOption, "", 32);
	// /* Cancel observe first */
	// requestObserver = 1; // Do not observe
	// submit_work_if_connected(&cancel_observe_work);
	// /* then delete */
	requestObserver = 1; // Do not observe
	submit_work_if_connected(&delete_role_work);
}

void coap_client_toggle_minimal_sleepy_end_device(void)
{
	if (IS_ENABLED(CONFIG_OPENTHREAD_MTD_SED)) {
		k_work_submit(&toggle_MTD_SED_work);
	}
}
