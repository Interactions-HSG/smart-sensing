package interactions.ics.unisg.ch.smartsensing.entities;

import com.google.gson.annotations.SerializedName;

import java.util.Objects;

public class GroupRoleInfo {
    @SerializedName(value = "id")
    public String id;
    @SerializedName(value = "cid")
    public String creatorId;
    @SerializedName(value = "act")
    public Boolean isActive = true;
    @SerializedName(value = "acs")
    public long isActiveSince = 0;
    @SerializedName(value = "fs")
    public FunctionalSpec functionalSpecification;
    @SerializedName(value = "mina")
    public int minAllocation = 100;

    @SerializedName(value = "cura")
    public int currentAllocation = 0;
    @SerializedName(value = "curg")
    public int currentAgents = 0;
    @SerializedName(value = "ming")
    public int minAgents = 1;
    @SerializedName(value = "maxg")
    public int maxAgents = 1;
    @SerializedName(value = "rew")
    public double reward = 0.0f;

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        GroupRoleInfo that = (GroupRoleInfo) o;
        return minAllocation == that.minAllocation && minAgents == that.minAgents && maxAgents == that.maxAgents && Objects.equals(isActive, that.isActive) && Objects.equals(functionalSpecification, that.functionalSpecification);
    }

    @Override
    public int hashCode() {
        return Objects.hash(isActive, functionalSpecification, minAllocation, minAgents, maxAgents);
    }
}
