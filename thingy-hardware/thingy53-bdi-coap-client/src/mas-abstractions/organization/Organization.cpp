#include "mas-abstractions/organization/Organization.h"

LOG_MODULE_REGISTER(Organization, CONFIG_COAP_CLIENT_UTILS_LOG_LEVEL);

static std::list<PlayerInfo*> currentRoles{};

void Organization::onDirectResponse(void* pContext,  uint8_t *p_message, uint16_t length)
{
    LOG_INF("%s"," Organization::onChannelResponse2");
    struct PlayerInfo pi;

    char* context = (char*)pContext;
    LOG_INF("Response context: %s",context);

    int ret = json_obj_parse((char *)p_message, length,
                PlayerInfo_descr,
                ARRAY_SIZE(PlayerInfo_descr),
                &pi);
    
    if (ret < 0)
    {
        LOG_ERR("JSON Parse Error: %d", ret);
    }
    else
    {
        LOG_INF("json_obj_parse return code: %d", ret);
        LOG_INF("id: %s", pi.id);
        LOG_INF("reward: %d", pi.reward);
    }
    dk_set_led_on(DK_LED3);
}

void Organization::refreshGroupRoles(std::string groupName)
{
    std::string uri = "room1";
    std::string param = "";
    char context[4] = "RGR";
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_GET, 0, context);
}

void Organization::refresh()
{
    std::string uri = "room1/gr_comfort_sensing/ag1";
    std::string param = "";
    char context[4] = "REF";
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_GET, 0, context);

    //struct ChannelRequest request;
    //request.serverUri = serverAddr;
    //request.resourceUri = "room1/gr_comfort_sensing/ag1";
    //request.action = CH_READ;
    //request.pCaller = this;
    //request.callback = onChannelMessage;
    //pChannel->sendRequest(request);
    LOG_INF("%s","Sent request to CoapServer");
}

std::list<PlayerInfo*> Organization::getMyRoles()
{
    return currentRoles;
}
