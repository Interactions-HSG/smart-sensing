package interactions.ics.unisg.ch.smartsensing.entities;

public class GroupRoleInfo {
    public String id;
    public String creatorId;


    public Boolean isActive = true;
    public long isActiveSince = 0;
    public FunctionalSpec functionalSpecification;
    public int minAllocation = 100;

    public int currentAllocation = 0;
    public int currentAgents = 0;
    public int minAgents = 1;
    public int maxAgents = 1;
    public double reward = 0.0f;
}
