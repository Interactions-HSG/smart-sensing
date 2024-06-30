#include "mas-abstractions/agent/ReactiveAgent.h"
//#include "peripherals/battery.h"

#define MIN_AVAILABLE_ENERGY 5.0

void ReactiveAgent::run(){
    //Organization::refresh();
    //advertise("room-1");
}

double ReactiveAgent::getEnergyInBuffer(){
    return 100;// (battery_sample() - 3000) / 1200.0f;
}

double ReactiveAgent::getEnergyCommitted(){
    std::list<PlayerInfo*> enacting = Organization::getMyRoles();
    double commited = 0.0f;
    for(PlayerInfo* r : enacting){
        commited += r->cost;
    }
    return commited;
}

double ReactiveAgent::calculateCost(GroupRoleInfoCon* group, double buffer, double committed){
    double e_m = group->fs.md * 0.15;
    double e_a = buffer - committed;
    double cost = e_m / e_a;
    return cost;
}

double ReactiveAgent::calculateBenefit(GroupRoleInfoCon* group, double buffer, double committed){
    double reward = group->rew;
    double cost = calculateCost(group, buffer, committed);
    double benefit = reward - cost;
    return benefit;
}

void ReactiveAgent::delibrate(){
    //Sum up energy required for current roles
    double e_c = getEnergyCommitted();
    double e_b = getEnergyInBuffer();
    //Spare energy? Get available roles
    if(e_b - e_c > MIN_AVAILABLE_ENERGY)
    {
        std::list<GroupRoleInfoCon*> availableRoles = Organization::getAvailableRoles();
        for(GroupRoleInfoCon* gr : availableRoles)
        {
            if(calculateBenefit(gr, e_b, e_c) > 0){
                Organization::joinRole(gr->id, 100);
                break; 
            }
        }
    }
}