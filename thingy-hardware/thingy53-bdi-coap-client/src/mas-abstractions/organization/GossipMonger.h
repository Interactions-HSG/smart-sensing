#ifndef __GOSSIP_MONGER_H__
#define __GOSSIP_MONGER_H__

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <iostream>
#include <list>
#include "mas-abstractions/organization/Organization.h"
#include "mas-abstractions/organization/GroupRole.h"
#include "coap/CoapClient.h"

static std::list<std::string*> fragments{};
void gossip(std::string fragment);
void advertise(std::string groupName);
void process(std::string fragment);

#endif