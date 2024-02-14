package organization_interface;

import com.google.gson.Gson;
import common.GlobalClock;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


//import org.eclipse.californium.core.CoapClient;

public class Organization {

    public interface OrganizationListener {
        void onGroupRoleInfoChange(String data);
        void onMeasurement(String groupId, String data);
    }
    static final CoapClient client = null;//= new CoapClient("coap://localhost:5683/room1");
    final static Object lock = new Object();

    static Gson gson = new Gson();

    static List<GroupRole> groupRoles = new ArrayList<>();

    public static void dump(){
        System.out.println("--------------------------- Summary -------------------------------------");
        System.out.println("id\t\tActive\tTrem\tNag\tAg0\tNm\tNme\tEff");
        for(GroupRole gr: groupRoles){
            int interval =  gr.getSpecification().functionalSpecification.measurementInterval;
            int expectedMeasurements = gr.getSpecification().functionalSpecification.measurementDuration * 60000 /interval;
            //double expectedPerMinute = (double)60000 /(double)interval;
            double fraction = (double)gr.getMeasurementCount() * 100.0f / expectedMeasurements;

            System.out.printf("%s\t%b\t%d\t%d\t%s\t%d\t%d\t%f\n", gr.specification.id, gr.specification.isActive, gr.specification.functionalSpecification.measurementDuration - (GlobalClock.ticks - gr.specification.isActiveSince), gr.specification.currentAgents,  gr.players.size() > 0 ? gr.players.get(0).id : "none", gr.getMeasurementCount(),expectedMeasurements, fraction );
        }
        System.out.println("--------------------------------------------------------------------------");
    }

    public static List<GroupRoleInfo> getGroupRoles() {
        List<GroupRoleInfo> groupRolesInfos = new ArrayList<>();
        for(GroupRole gr :groupRoles){
            String data = gson.toJson(gr.getSpecification());
            GroupRoleInfo grinfo  = gson.fromJson(data, GroupRoleInfo.class);
            groupRolesInfos.add(grinfo);
        }
        return groupRolesInfos;
    }

    public static GroupRoleInfo getGroupRole(String groupRoleId) {
        for(GroupRole role : groupRoles){
            if(role.getSpecification().id.equals((groupRoleId))) {
                String data = gson.toJson(role.getSpecification());
                GroupRoleInfo grinfo = gson.fromJson(data, GroupRoleInfo.class);
                return grinfo;
            }
        }
        return  null;
    }

    public static boolean joinGroupRole(String groupRoleId, PlayerInfo player) {
        for(GroupRole gr : groupRoles)
        {
            if(gr.getSpecification().id.equals(groupRoleId)){
                return gr.addPlayer(player);
            }
        }
        Organization.dump();
        return false;
    }

    public static boolean leaveGroupRole(String groupRoleId, PlayerInfo player) {
        for(GroupRole gr : groupRoles)
        {
            if(gr.getSpecification().id.equals(groupRoleId)){
                gr.removePlayer(player);
                break;
            }
        }
        Organization.dump();
        return true;
    }

    public static boolean createGroupRole(GroupRoleInfo roleInfo){
        groupRoles.add(new GroupRole(roleInfo));
        return true;
    }

    public static boolean updateGroupRole(GroupRoleInfo roleInfo){
        for(GroupRole gr : groupRoles)
        {
            if(gr.getSpecification().id.equals(roleInfo.id)){
                gr.updateSpecification(roleInfo);
                break;
            }
        }
        Organization.dump();
        return true;
    }

    public static boolean deleteGroupRole(String groupRoleId){
        for(int i=0; i<groupRoles.size(); i++){
            if(groupRoles.get(i).getSpecification().id.equals(groupRoleId)){
                groupRoles.remove(i);
                return true;
            }
        }
        return false;
    }

    static List<OrganizationListener> organizationListeners2 = new ArrayList<>();
    public static void observeGroupRole(String groupRoleId, OrganizationListener listener) {
        organizationListeners2.add(listener);
    }

    static Map<String, OrganizationListener> measurementListeners2 = new HashMap<>();
    public static void observeMeasurements(String groupRoleId, OrganizationListener listener){
        measurementListeners2.put(groupRoleId, listener);
    }


    public static boolean sendMeasurement(String roleId, double value) {
        measurementListeners2.get(roleId).onMeasurement(roleId, String.format("%f",value));
        for(int i=0; i<groupRoles.size(); i++){
            if(groupRoles.get(i).getSpecification().id.equals(roleId)){
                groupRoles.get(i).receiveMeasurement("x");
                return true;
            }
        }
        return true;
    }

    public static List<GroupRoleInfo> getGroupRoles_(){
        // synchronous
        String response = null;
        try {
           {
                client.setURI("coap://localhost:5683/room1");
                client.useNONs();
                CoapResponse coapResponse = client.get();
                if (coapResponse == null) {
                    System.out.println("No response from server for " + client.getURI());
                    return null;
                }
                response = coapResponse.getResponseText();
               Gson gson1 = new Gson();
                GroupRoleInfos roles = gson1.fromJson(response, GroupRoleInfos.class);
                if (roles == null) {
                    return null;
                }
                client.shutdown();
                return roles.elements;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static GroupRoleInfo getGroupRole_(String groupRoleId){

        // synchronous
        String response = null;
        try {
            {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId);
                CoapResponse coapResponse = client.get();
                if (coapResponse == null) {
                    System.out.println("No response from server for " + client.getURI());
                    return null;
                }
                response = coapResponse.getResponseText();
                //System.out.println("Get group info: " + response);
                GroupRoleInfo role = gson.fromJson(response, GroupRoleInfo.class);
                return role;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    static CoapObserveRelation groupObserver;

    static List<OrganizationListener> organizationListeners = new ArrayList<>();
    public static void observeGroupRole_(String groupRoleId, OrganizationListener listener){
        organizationListeners.add(listener);
        if(groupObserver != null){
            return;
        }
        {
            client.setURI("coap://localhost:5683/room1");
            //if(groupObserver != null && !groupObserver.isCanceled()){
            //   groupObserver.proactiveCancel();
            //}
            groupObserver = client.observe(
                    new CoapHandler() {
                        @Override
                        public void onLoad(CoapResponse response) {
                            String content = response.getResponseText();
                            //System.out.println("Observe Notification: " + content);
                            for (OrganizationListener l : organizationListeners) {
                                l.onGroupRoleInfoChange(content);
                            }
                        }

                        @Override
                        public void onError() {
                            System.err.println("OBSERVING FAILED (press enter to exit)");
                        }
                    });
        }

        //relation.proactiveCancel();
    }

    //static CoapObserveRelation measurementObserver;

    public static void cancelMeasurementObservation(){

    }
    public static void observeMeasurements_(String groupRoleId, OrganizationListener listener){
       {
            client.setURI("coap://localhost:5683/room1/" + groupRoleId + "/receiver");

            CoapObserveRelation observer = client.observe(
                    new CoapHandler() {
                        @Override
                        public void onLoad(CoapResponse response) {
                            String content = response.getResponseText();
                            //System.out.println("Observe Notification: " + content);
                            listener.onMeasurement(groupRoleId, content);
                        }

                        @Override
                        public void onError() {
                            System.err.println("OBSERVING FAILED (press enter to exit)");
                        }
                    });

            //relation.proactiveCancel();
        }
    }

    public static boolean joinGroupRole_(String groupRoleId, PlayerInfo player){

        // synchronous
        CoapResponse response = null;
        try {
            synchronized (client) {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId);
                String data = gson.toJson(player);
                response = client.post(data, MediaTypeRegistry.APPLICATION_JSON);
                if(response.getCode() == CoAP.ResponseCode.CREATED) {
                    System.out.println("Added role player: " + player.id + " to " + groupRoleId);
                }
                return response.getCode() == CoAP.ResponseCode.CREATED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean leaveGroupRole_(String groupRoleId, PlayerInfo player){
        CoapResponse response = null;
        try {
            {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId + "/" + player.id);
                response = client.delete();
                System.out.println("Deleted role player: " + player.id + " from " + groupRoleId);
                return response.getCode() == CoAP.ResponseCode.DELETED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean createGroupRole_(GroupRoleInfo roleInfo){

        // synchronous
        CoapResponse response = null;
        try {
            {
                client.setURI("coap://localhost:5683/room1/");
                String data = gson.toJson(roleInfo);
                response = client.post(data, MediaTypeRegistry.APPLICATION_JSON);
                System.out.println("Added group role: " + roleInfo.id);
                return response.getCode() == CoAP.ResponseCode.CREATED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean updateGroupRole_(GroupRoleInfo roleInfo){
        CoapResponse response = null;
        try {
            {
                client.setURI("coap://localhost:5683/room1/" + roleInfo.id);
                String data = gson.toJson(roleInfo);
                response = client.put(data, MediaTypeRegistry.APPLICATION_JSON);
                //System.out.println("Update role player: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.CHANGED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean deleteGroupRole_(String groupRoleId){
        CoapResponse response = null;
        try {
            synchronized (client) {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId );
                response = client.delete();
                System.out.println("Deleted group role: " + groupRoleId);
                return response.getCode() == CoAP.ResponseCode.DELETED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean sendMeasurement_(String roleId, double value){

        // synchronous
        CoapResponse response = null;
        try {
            {
                client.setURI("coap://localhost:5683/room1/" + roleId + "/receiver");
                String data = gson.toJson(value);
                response = client.post(data, MediaTypeRegistry.APPLICATION_JSON);
                //System.out.println("Add role player: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.CREATED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
