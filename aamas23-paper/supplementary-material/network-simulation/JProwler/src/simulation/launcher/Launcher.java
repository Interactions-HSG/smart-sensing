package simulation.launcher;

import com.google.gson.Gson;
import net.tinyos.prowler.*;
import organization_interface.Group;

import java.util.ArrayList;
import java.util.List;

public class Launcher {

    private static int nodeId = 1;
    //Simulation parameters
    static int numberOfRooms = 4;
    static int motesPerRoom = 3;
    //The geographical space is 25 x 25 units - top left is 0,0
    static int[] roomXPos = {1,2,3,4};
    static int[] roomYPos = {1,2,3,4};
    //The position of the router can be changed here:
    static int rootNodeXpos = 10;
    static int rootNodeYpos = 10;


    public static void main(String[] args) throws Exception{

        System.out.println("Preparing WSAN simulator");

        //------------ Setup Network Simulator ---------------
        Simulator sim = new Simulator();
        RayleighRadioModel radioModel = new RayleighRadioModel(sim);
        radioModel.radioStrengthCutoff = 0.2;
        radioModel.fallingFactorHalf = 1.1;


        //------------ Create the floor-level border router node (sink node) --------------
        long time0 = System.currentTimeMillis();
        OrgGossiperNode root = (OrgGossiperNode)sim.createNode( OrgGossiperNode.class, radioModel, 0, rootNodeXpos, rootNodeYpos, 0);
        root.nodeType = "sink";
        ArrayList<String> floorGroup = new ArrayList<String>();
        floorGroup.add("floor-1");
        root.setAffliatedGroupNames(floorGroup);
        OrgGossiperNode.GossipApplication rootApp = root.new GossipApplication(root);
        //The corresponding group in the organization
        Group rootGroup = new Group(String.format("floor-%d", 1));

        //For each room, we add three sub-groups
        //and each room has motesPerRoom sensors
        //a sensor "belongs" to a room and is free to serve any measurement role relevant to actors in that room.
        for(int i = 0; i < numberOfRooms; i++) {
            Group roomGroup = new Group(String.format("room-%d", i));
            Group heatingGroup = new Group(String.format("heating-system"));
            Group lightingGroup = new Group(String.format("lighting-system"));
            Group monitoringGroup = new Group(String.format("monitoring-system"));
            roomGroup.subGroups.add(heatingGroup);
            roomGroup.subGroups.add((lightingGroup));
            roomGroup.subGroups.add((monitoringGroup));
            rootGroup.subGroups.add(roomGroup);

            ArrayList<String> sensorsInRoom = new ArrayList<>();
            sensorsInRoom.add(String.format("room-%d", i));
            createMotes(sim, radioModel, sensorsInRoom, motesPerRoom, i);
        }

        radioModel.updateNeighborhoods();

        System.out.println("Creation time: " + (System.currentTimeMillis()-time0) + " millisecs" );
        final long time1 = System.currentTimeMillis();
        System.out.println("Starting simulation");
        //Seed organization description in the root node
        Gson gson = new Gson();
        //root.sendMessage( gson.toJson(rootGroup), bcApp );
        rootApp.seed(gson.toJson(rootGroup));
        boolean realTime = true;
        if( realTime )
        {
            sim.runWithDisplay();
        }

    }


    //Helper method to create the motes and provision it
    private static List<OrgGossiperNode> createMotes(Simulator sim, RadioModel radioModel, ArrayList<String> groupIds, int numberOfMotes, int groupNumber) throws Exception {
        List<OrgGossiperNode> gossipers = new ArrayList<>();
        Node newNode = sim.createNodes( OrgGossiperNode.class, radioModel, nodeId, nodeId+numberOfMotes, 25, 0);
        while (newNode != null){
            OrgGossiperNode mote = ((OrgGossiperNode)newNode);
            mote.setAffliatedGroupNames(groupIds);
            mote.setNodeId(String.format("node_%d", nodeId++));
            //mote.setX(roomXPos[groupNumber] + (2 * Math.random()));
            //mote.setY(roomYPos[groupNumber] + (2 * Math.random()));
            gossipers.add(mote);
            OrgGossiperNode.GossipApplication moteApp = mote.new GossipApplication(mote);
            newNode = newNode.nextNode;
        }
        return gossipers;
    }
}
