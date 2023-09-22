package simulation.launcher;

import com.google.gson.Gson;
import net.tinyos.prowler.*;
import organization_interface.Group;

import java.util.ArrayList;
import java.util.List;

public class Launcher {

    private static int nodeId = 1;
    private static List<OrgGossiperNode> createMotes(Simulator sim, RadioModel radioModel, ArrayList<String> groupIds, int number) throws Exception {
        List<OrgGossiperNode> gossipers = new ArrayList<>();
        Node tempNode = sim.createNodes( OrgGossiperNode.class, radioModel, nodeId, nodeId+number, 50, 0);

        while (tempNode != null){
            ((OrgGossiperNode)tempNode).setAffliatedGroupNames(groupIds);
            ((OrgGossiperNode)tempNode).setNodeId(String.format("node_%d", nodeId++));
            gossipers.add(((OrgGossiperNode)tempNode));
            OrgGossiperNode.GossipApplication tempBcApp = ((OrgGossiperNode)tempNode).new GossipApplication(tempNode);
            tempNode = tempNode.nextNode;
        }
        return gossipers;
    }
    public static void main(String[] args) throws Exception{

        Gson gson = new Gson();
        System.out.println("creating wsan nodes...");
        Simulator sim = new Simulator();

        RayleighRadioModel radioModel = new RayleighRadioModel(sim);
        radioModel.radioStrengthCutoff = 0.2;
        radioModel.fallingFactorHalf = 1.1;

        // creating a fully-powered node in the middle of the field, and adding a gossip
        // application
        long time0 = System.currentTimeMillis();
        OrgGossiperNode root = (OrgGossiperNode)sim.createNode( OrgGossiperNode.class, radioModel, 1, 25, 25, 0);
        root.nodeType = "sink";
        OrgGossiperNode.GossipApplication bcApp = root.new GossipApplication(root);

        // creating all the other motes

        ArrayList<String> heaters = new ArrayList<>();
        heaters.add("heating-system");
        createMotes(sim, radioModel, heaters, 5);

        ArrayList<String> heatersandlighters = new ArrayList<>();
        heatersandlighters.add("heating-system");
        heatersandlighters.add("lighting-system");
        createMotes(sim, radioModel, heatersandlighters, 5);

        ArrayList<String> lighters = new ArrayList<>();
        lighters.add("lighting-system");
        createMotes(sim, radioModel, lighters, 5);

        radioModel.updateNeighborhoods();

        System.out.println("creation time: " + (System.currentTimeMillis()-time0) + " millisecs" );
        final long time1 = System.currentTimeMillis();

        System.out.println("start simulation");

        Group rootGroup = new Group("room-1");
        Group heatingGroup = new Group("heating-system");
        Group lightingGroup = new Group("lighting-system");
        rootGroup.subGroups.add(heatingGroup);
        rootGroup.subGroups.add((lightingGroup));

        root.sendMessage( gson.toJson(rootGroup), bcApp );


        boolean realTime = true;
        if( realTime )
        {
            sim.runWithDisplay();
        }

    }
}
