/**
 * coap_client.h
 */

#ifndef __COAP_CLIENT_H__
#define __COAP_CLIENT_H__

#include <coap_server_client_interface.h>

#include <stdio.h>
#include <zephyr/kernel.h>
#include <net/coap_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/openthread.h>
//#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include "mas-abstractions/organization/Organization.h"
extern "C" {
  //#include "peripherals/sensors.h"
  //#include "peripherals/battery.h"
  #include "peripherals/gpio.h"
  #include "thread/thread_helper.h"
}


/**************************************************************************************************
  IPv6 Address of CoAP Server
**************************************************************************************************/
static const char *const serverAddress = "fd19:99a6:cda:f9e6:0:ff:fe00:fc11"; //fd19:99a6:cda:f9e6:0:ff:fe00:fc11


typedef void (*coap_response_cb_t)(void *p_context,  otMessage *p_message,  const otMessageInfo *p_message_info, otError result);

struct CoapRequest {
    const char* serverAddress;
    const char* uriPath;
    const char* queryOption;
    otCoapType requestType;
    otCoapCode requestCode;
    uint32_t requestObserver;
    coap_response_cb_t responseCallback;
    void* aContext;
};

class CoapClient
{
    public:
        static otError initialize();
        static otError sendRequest2(const char* uri, const char* query, const char* payload, otCoapType requestType, otCoapCode requestCode, uint32_t requestObserve, void* pContext);
        static otError cleanup();
    private:
        static void coap_response_handler(void *p_context,	otMessage *p_message,const otMessageInfo *p_message_info, otError aResult);
        static void coap_default_handler(void *p_context,	otMessage *p_message,const otMessageInfo *p_message_info);
        static otError init_coap_message(otMessage *aMessage, const char *aUriPath, const char *aQuery, const char* payload, otCoapType aType, 	  otCoapCode aCode, uint32_t aObserve);
};

#endif