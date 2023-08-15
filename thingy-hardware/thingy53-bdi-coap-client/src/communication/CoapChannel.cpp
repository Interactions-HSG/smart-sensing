#include "communication/AbstractChannel.h"
#include "communication/CoapChannel.h"
#include "mas-abstractions/organization/Organization.h"

LOG_MODULE_REGISTER(CoapChannel, CONFIG_SENSOR_LOG_LEVEL);

static uint8_t received_data[1024];

static ChannelClient *pChannelClient;

//(void *p_context,  otMessage *p_message,  const otMessageInfo *p_message_info, otError result);
void CoapChannel::coap_default_handler(void *p_context,
								otMessage *p_message,
								const otMessageInfo *p_message_info, otError result)
{
	ARG_UNUSED(p_message_info);
	ARG_UNUSED(p_message);
	ARG_UNUSED(p_context);


    uint16_t received_size;

	if (result == OT_ERROR_NONE) {
		LOG_INF("%i: Received GET REWARD successfully.\n", 0);
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

    LOG_INF("%s", "Getting channel request from context");
    ChannelRequest* pRequest = (ChannelRequest*)p_context;
    LOG_INF("%s", "Calling response method on channel");
    //(void *p_context,  char *p_message,  char *p_uri, channelResponse result)
    uint8_t slice[length+1];
    memcpy(slice, received_data, length+1);
    //Organization::onChannelResponse2(pRequest, slice, length+1, CH_SUCCESS );
	//LOG_WRN("Received CoAP message that does not match any resource!\n");

}

channelResponse CoapChannel::initialize(ChannelClient *pChannel)
{
    pChannelClient = pChannel;
    otError error = client.initialize();
    //k_work_init(&unicast_request_work, unicast_request_work_handler);
    return error == OT_ERROR_NONE ? CH_SUCCESS : CH_ERROR_FAILED;
}


channelResponse CoapChannel::cleanup()
{
    otError error = client.cleanup();
    return error == OT_ERROR_NONE ? CH_SUCCESS : CH_ERROR_FAILED;
}

void CoapChannel::unicast_request_work_handler(struct k_work *item){
    LOG_INF("%s", "unicast_request_work_handler invoked");
    struct coap_work_request *the_request = CONTAINER_OF(item, struct coap_work_request, work);
    CoapClient* pClient = the_request->pCoapClient;
    LOG_INF("%s", "unicast_request_work_handler invoked - calling coap client with request");
    otError error = pClient->sendRequest(the_request->pCoapRequest);
}

void CoapChannel::submit_work_if_connected(struct k_work *work)
{
	if (true) {
        LOG_INF("%s", "submitting request work");
		k_work_submit(work);
	} else {
		LOG_INF("%s","Connection is broken");
	}
}

channelResponse CoapChannel::sendRequest(ChannelRequest channelRequest){
    LOG_INF("%s", "sendRequest received");
    channelResponse chResp = CH_SUCCESS;
    CoapRequest coapReq;
    coapReq.serverAddress = channelRequest.serverUri;
    //strncpy(coapReq.uriPath, request.resourceUri, 180);
    coapReq.uriPath = channelRequest.resourceUri;
    coapReq.queryOption = "";
    coapReq.requestCode = OT_COAP_CODE_GET;
    coapReq.requestType = OT_COAP_TYPE_CONFIRMABLE;
    coapReq.requestObserver = 0;
    coapReq.aContext = (void*)&channelRequest;
    coapReq.responseCallback = coap_default_handler;

    struct coap_work_request req;
    req.pCoapRequest = &coapReq;
    LOG_INF("%s", "formulated coap request. initing work and sending..");
    k_work_init(&req.work, unicast_request_work_handler);
    submit_work_if_connected(&req.work);    
    return chResp;
}

