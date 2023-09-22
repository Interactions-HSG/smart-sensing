#ifndef __ORGANIZATION_H__
#define __ORGANIZATION_H__

#include <zephyr/logging/log.h>
#include <dk_buttons_and_leds.h>
#include <iostream>
#include <list>
#include "mas-abstractions/organization/GroupRole.h"
#include "coap/CoapClient.h"

class Organization 
{

    public:
        Organization();

        /**
         * This function returns GroupRoles known in the organization after the last refresh.
         *
         * @returns A list of GroupRoles that are available in the group to which this agent belongs.
         *
         */
        static std::list<PlayerInfo*> getMyRoles();
        static void refresh();
        static void refreshGroupRoles(std::string groupName);
        static void joinRole(std::string groupRoleName);
        static void leaveRole(std::string groupRoleName);
        static void onDirectResponse(void* pContext,  uint8_t *p_message, uint16_t length);
        static void sendMeasurement(std::string groupName, double value);
    private:
        
        
};

#endif