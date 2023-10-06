#include "mas-abstractions/organization/Organization.h"

LOG_MODULE_REGISTER(Organization, CONFIG_COAP_CLIENT_UTILS_LOG_LEVEL);

static std::list<PlayerInfo*> currentRoles{};
static std::list<GroupRoleInfo*> availableRoles{};

static uint8_t ctx_read_player_info = 0;
static uint8_t ctx_read_group_roles = 1;
static uint8_t ctx_read_group_role = 2;
static uint8_t ctx_join_group_role = 3;
static uint8_t ctx_leave_group_role = 4;
static uint8_t ctx_send_measurement = 5;

void Organization::onDirectResponse(void* pContext,  uint8_t *p_message, uint16_t length)
{
    LOG_INF("%s"," Organization::onChannelResponse2");
    uint8_t* context = (uint8_t*)pContext;

    if(context == &ctx_read_group_roles){
        struct GroupRoleInfos grinfos;
        //See: https://blog.golioth.io/how-to-parse-json-data-in-zephyr/
        int ret = json_obj_parse((char *)p_message, length,
                    GroupRoleInfos_descr,
                    ARRAY_SIZE(GroupRoleInfos_descr),
                    &grinfos);
        
        if (ret < 0)
        {
            LOG_ERR("JSON Parse Error: %d", ret);
        }else{
            availableRoles.clear();
            for(int i=0; i < grinfos.num_elements; i++)
            {
                availableRoles.push_back(&grinfos.elements[i]);
            }           
        }
    }else if (context == &ctx_read_player_info){
        struct PlayerInfo pi;
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
            //LOG_INF("reward: %d", pi.reward);
        }
    }else{
        LOG_INF("Context: %d", *context);
    }


    dk_set_led_on(DK_LED3);
}

void Organization::refreshGroupRoles(std::string groupName)
{
    std::string uri = groupName;
    std::string param = "";
    std::string payload = "";
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), payload.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_GET, 1, &ctx_read_group_roles);
}

void Organization::sendMeasurement(std::string groupRoleName, double value)
{
    std::string uri = "room1/"+ groupRoleName + "/receiver";
    std::string param = "";
    std::string payload = "12.1";
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), payload.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_POST, 0, &ctx_send_measurement);
}

void Organization::joinRole(std::string groupRoleName)
{
    std::string uri = "room1/"+ groupRoleName;
    std::string param = "";
    PlayerInfo player;
    player.id = (char*)"sen_01";
    player.taskAllocation = 100;
    player.cost = 0.9f;
    player.networkCost = 0.1f;
    player.reward = 1.0f;
    
    char buffer[100];
    int ret = json_obj_encode_buf(PlayerInfo_descr, ARRAY_SIZE(PlayerInfo_descr), &player, buffer, sizeof(buffer));
    std::string payload(buffer);
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), payload.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_POST, 0, &ctx_join_group_role);
}

void Organization::leaveRole(std::string groupRoleName)
{
    std::string uri = "room1/"+ groupRoleName + "/sen_01";
    std::string param = "";
    std::string payload = "";
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), payload.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_DELETE, 0, &ctx_leave_group_role);
}

void Organization::refresh()
{
    refreshGroupRoles("room1");
    /*
    std::string uri = "room1/gr_comfort_sensing";
    std::string param = "";
    char context[4] = "REF";
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_GET, 0, context);
*/
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

std::list<GroupRoleInfo*> Organization::getAvailableRoles()
{
    return availableRoles;
}