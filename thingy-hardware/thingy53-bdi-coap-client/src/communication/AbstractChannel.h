#ifndef __ABSTRACTCHANNEL_H__
#define __ABSTRACTCHANNEL_H__

#include <stdint.h>


typedef enum channelResponse
{
    /**
     * No error.
     */
    CH_SUCCESS = 0,

    /**
     * Operational failed.
     */
    CH_ERROR_FAILED = 1
} channelResponse;

typedef enum channelAction
{
    CH_READ = 0,
    CH_UPDATE = 1,
    CH_CREATE = 2,
    CH_DELETE = 3,
    CH_SUBSCRIBE = 4,
    CH_UNSUBSCRIBE = 5,
    CH_NOTIFY = 6
}channelAction;


typedef void (*channel_callback_t)(void *p_request,  char *p_message, channelResponse result);

class ChannelClient;

struct ChannelRequest
{
    void* aContext;
    channel_callback_t callback;
    ChannelClient *pCaller;
    channelAction action;
    const char* resourceUri;
    const char* serverUri;
    const char* data;
};

class ChannelClient
{
    public:
        virtual void onChannelResponse(ChannelRequest *p_request,  uint8_t *p_message, channelResponse result) = 0;
};

class AbstractChannel
{

    public:
        virtual channelResponse initialize(ChannelClient *pClient) = 0;
        virtual channelResponse cleanup() = 0;
        virtual channelResponse sendRequest(ChannelRequest request) = 0;
};



#endif