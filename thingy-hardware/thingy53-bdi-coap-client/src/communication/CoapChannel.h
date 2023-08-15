#ifndef __COAPCHANNEL_H__
#define __COAPCHANNEL_H__

#include "communication/AbstractChannel.h"
#include "coap/CoapClient.h"

struct coap_work_request {
    struct k_work work;
    CoapClient* pCoapClient;
    CoapRequest* pCoapRequest;
} ;

class CoapChannel: public AbstractChannel
{

    public:
        channelResponse initialize(ChannelClient *pClient);
        channelResponse cleanup();
        channelResponse sendRequest(ChannelRequest request);
    private:
        static void coap_default_handler(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result);
        CoapClient client;
        //static ChannelClient *pChannelClient;
        static struct k_work unicast_request_work;
        static void submit_work_if_connected(struct k_work *work);
        static void unicast_request_work_handler(struct k_work *item);
};

#endif