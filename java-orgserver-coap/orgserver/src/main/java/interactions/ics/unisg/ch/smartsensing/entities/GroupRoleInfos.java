package interactions.ics.unisg.ch.smartsensing.entities;

import com.google.gson.annotations.SerializedName;

import java.util.List;

public class GroupRoleInfos{
    @SerializedName(value = "e")
    public List<GroupRoleInfo> elements;
    @SerializedName(value = "n")
    public int num_elements;
}