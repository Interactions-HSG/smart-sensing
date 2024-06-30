#include "coap/CoapClient.h"
#define OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE 1
LOG_MODULE_REGISTER(CoapClient, CONFIG_SENSOR_LOG_LEVEL);

static char uriPath[180];
static char queryOption[180];
static uint8_t received_data[1024];

/**************************************************************************************************
  Default handler for incoming CoAP messages.
**************************************************************************************************/
void CoapClient::coap_default_handler(void *p_context,
								otMessage *p_message,
								const otMessageInfo *p_message_info)
{
	ARG_UNUSED(p_message_info);
	ARG_UNUSED(p_message);
	ARG_UNUSED(p_context);

}


/**************************************************************************************************
  Default handler for response CoAP messages.
**************************************************************************************************/
void CoapClient::coap_response_handler(void *p_context,
								otMessage *p_message,
								const otMessageInfo *p_message_info, otError aResult)
{
	ARG_UNUSED(p_message_info);
	ARG_UNUSED(p_message);
	ARG_UNUSED(p_context);

	uint16_t received_size;
	gpio_output_set(8,1);

	if (aResult == OT_ERROR_NONE) {
		LOG_INF("%i: Received response successfully.\n", 0);
	}
	else {
		LOG_WRN("Delivery not confirmed!\n");
	}

	uint16_t length = otMessageGetLength(p_message) - otMessageGetOffset(p_message);

	otCoapType type = otCoapMessageGetType(p_message);
	LOG_INF("With code: %s\n",  otCoapMessageCodeToString(p_message));
	if (type != OT_COAP_TYPE_ACKNOWLEDGMENT &&
		type != OT_COAP_TYPE_CONFIRMABLE) {
		LOG_WRN("Received unexpected CoAP type.\n");
	}

	otCoapCode code = otCoapMessageGetCode(p_message);
	if (code != OT_COAP_CODE_CONTENT) {
		LOG_WRN("Received unexpected CoAP code.\n");
	}

	received_size = otMessageRead(p_message, otMessageGetOffset(p_message), received_data, length);
	received_data[length] = '\0';

	if (received_data == NULL) {
		LOG_ERR("Received reward data is invalid");
	}

    LOG_INF("Received: %s", received_data);
    //(void *p_context,  char *p_message,  char *p_uri, channelResponse result)
    uint8_t slice[length+1];
    memcpy(slice, received_data, length+1);
    Organization::onDirectResponse(p_context, slice, length+1);
	//LOG_WRN("Received CoAP message that does not match any resource!\n");

}

/**************************************************************************************************
  Generic function to initialize CoAP message.
**************************************************************************************************/


otError CoapClient::initialize()
{
    otError error;
    otInstance *p_instance = openthread_get_default_instance();
	/* Create CoAP instnce and start CoAP */
	error = otCoapStart(p_instance, COAP_PORT);
	if (error!=OT_ERROR_NONE){
		LOG_ERR("Failed to start Coap: %d\n", error);
        return error;
	}
	/* Assign CoAP default handler */
	otCoapSetDefaultHandler(p_instance, coap_default_handler, NULL);
    return error;
}

otError CoapClient::cleanup()
{
    otError error;
    otInstance *p_instance = openthread_get_default_instance();
	/* Create CoAP instnce and start CoAP */
	error = otCoapStop(p_instance);
	if (error!=OT_ERROR_NONE){
		LOG_ERR("Failed to stop Coap: %d\n", error);
	}
    return error;
}

otError CoapClient::init_coap_message(otMessage *aMessage, const char *aUriPath, const char *aQuery, const char* payload, otCoapType aType, 
						  otCoapCode aCode, uint32_t aObserve)
{	
	
	otError error = OT_ERROR_NONE;
	
	otCoapMessageInit(aMessage, aType, aCode);
	otCoapMessageGenerateToken(aMessage, OT_COAP_DEFAULT_TOKEN_LENGTH);
	
	#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
	if (aObserve == 1){		
		error = otCoapMessageAppendObserveOption(aMessage, 0);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("%s","Append Observe Option Error\n");
			return error;
		}
	}
	#endif

	error = otCoapMessageAppendUriPathOptions(aMessage, aUriPath);
	if (error != OT_ERROR_NONE){ 
		LOG_ERR("%s","URI PATH FAIL\n");
		return error;
	}

	error = otCoapMessageAppendContentFormatOption(aMessage, 
										OT_COAP_OPTION_CONTENT_FORMAT_JSON);
	if (error != OT_ERROR_NONE){ 
		LOG_ERR("%s","Content Error\n");
		return error;
	}

	if (strlen(aQuery) != 0){
		error = otCoapMessageAppendUriQueryOption(aMessage, aQuery);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("%s","QUERY OPTION FAIL\n");
			return error;
		}
	}
	
	if(strlen(payload) != 0){
		error = otCoapMessageSetPayloadMarker(aMessage);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("%s","Set Payload Marker Error\n");
			return error;
		}
		error = otMessageAppend(aMessage, payload, strlen(payload));
	}
	return error;
}

otError CoapClient::sendRequest2(const char* uri, const char* query, const char* payload, otCoapType requestType, otCoapCode requestCode, uint32_t requestObserve, void* pContext)
{
    //ARG_UNUSED(item);
	otError 		error = OT_ERROR_NONE;
	otMessage 		*myMessage;
	otMessageInfo 	myMessageInfo;
	otInstance 		*myInstance = openthread_get_default_instance();

	#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    bool           coapBlock     = false;
    otCoapBlockSzx coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_16;
    BlockType      coapBlockType = (aCoapCode == OT_COAP_CODE_GET) ? kBlockType2 : kBlockType1;
	#endif
	#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE && OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    if (aCoapObserve)
    {
        coapBlockType = kBlockType1;
    }
	#endif	

	strncpy(uriPath, uri, 180);
	strncpy(queryOption, query, 180);
	
	do{
		LOG_INF("%s","Creating new message");
		myMessage = otCoapNewMessage(myInstance, NULL);
		
		if (myMessage == NULL) {
			LOG_ERR("%s","Failed to allocate message for CoAP Request\n");
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		otIp6AddressFromString(serverAddress, &myMessageInfo.mPeerAddr);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("%s","Parse IPv6 Error\n");
			break;
		}
		error = init_coap_message(myMessage, uriPath, queryOption, payload, requestType, requestCode, requestObserve);
		if (error != OT_ERROR_NONE){ 
			LOG_ERR("%s","CoAP message initialization error!\n");
			break;
		}
		LOG_INF("%s","Sending message");
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, coap_response_handler, pContext);		
	}
	while(false);

	if (error != OT_ERROR_NONE) {
		LOG_WRN("Failed to send GET ROLE request: %d\n", error);
		otMessageFree(myMessage);
	}
	else{
		//LOG_INF("%i: GET REWARD sent success\n", log_iterator++);
		LOG_INF("%s","Send sucessful!");
	}
	return error;
}