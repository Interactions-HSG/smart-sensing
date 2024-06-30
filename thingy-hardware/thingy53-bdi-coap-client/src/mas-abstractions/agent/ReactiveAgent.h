#include "mas-abstractions/organization/Organization.h"
#include "mas-abstractions/organization/GossipMonger.h"

class ReactiveAgent
{
    public:
        void run();
    private:
        double getEnergyInBuffer();
        double getEnergyCommitted();
        void delibrate();
        double calculateBenefit(GroupRoleInfoCon* group, double buffer, double committed);
        double calculateCost(GroupRoleInfoCon* group, double buffer, double committed);
};