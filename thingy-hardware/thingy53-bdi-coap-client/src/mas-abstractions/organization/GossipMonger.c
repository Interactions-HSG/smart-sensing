#include "GossipMonger.h"


void gossip(std::string fragment)
{
    std::string uri = "room1/gossip-room";
    std::string param = "";
    std::string payload = fragment;
    CoapClient::sendRequest2(uri.c_str(), param.c_str(), payload.c_str(), otCoapType::OT_COAP_TYPE_CONFIRMABLE, otCoapCode::OT_COAP_CODE_POST, 0, &ctx_send_gossip);
}

void process(std::string fragment)
{
    fragments.append(&fragment);
}

void advertise(std::string groupName)
{
    gossip("ADV;" + groupName)
}