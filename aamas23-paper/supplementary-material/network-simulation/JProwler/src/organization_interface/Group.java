package organization_interface;

import java.util.ArrayList;
import java.util.List;

public class Group {
    public List<Group> subGroups = new ArrayList<>();

    public String name;


    public Group(String name){
        this.name = name;
    }

}
