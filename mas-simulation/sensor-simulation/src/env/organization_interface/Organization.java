package organization_interface;

import com.google.gson.Gson;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;

import java.io.IOException;
import java.util.List;


//import org.eclipse.californium.core.CoapClient;

public class Organization {

    public interface OrganizationListener {
        void onGroupRoleInfoChange(String data);
    }
    static CoapClient client = new CoapClient("coap://localhost:5683/room1");
    static final Object lock = new Object();

    static Gson gson = new Gson();

    public static List<GroupRole.GroupRoleInfo> getGroupRoles(){


        // synchronous
        String response = null;
        try {
            client.setURI("coap://localhost:5683/room1");
            response = client.get().getResponseText();
            //System.out.println("Get group info: " + response);
            GroupRole.GroupRoleInfos roles = gson.fromJson(response, GroupRole.GroupRoleInfos.class);
            return roles.elements;
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static GroupRole.GroupRoleInfo getGroupRole(String groupRoleId){

        // synchronous
        String response = null;
        try {
            client.setURI("coap://localhost:5683/room1/" + groupRoleId);
            response = client.get().getResponseText();
            //System.out.println("Get group info: " + response);
            GroupRole.GroupRoleInfo role = gson.fromJson(response, GroupRole.GroupRoleInfo.class);
            return role;
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    static CoapObserveRelation groupObserver;
    public static void observeGroupRole(String groupRoleId, OrganizationListener listener){
        client.setURI("coap://localhost:5683/room1/" + groupRoleId);
        if(groupObserver != null && !groupObserver.isCanceled()){
            groupObserver.proactiveCancel();
        }
        groupObserver = client.observe(
                new CoapHandler() {
                    @Override public void onLoad(CoapResponse response) {
                        String content = response.getResponseText();
                        System.out.println("Observe Notification: " + content);
                        listener.onGroupRoleInfoChange(content);
                    }

                    @Override public void onError() {
                        System.err.println("OBSERVING FAILED (press enter to exit)");
                    }
                });

        //relation.proactiveCancel();
    }

    public static boolean joinGroupRole(String groupRoleId, RolePlayer.PlayerInfo player){

        // synchronous
        CoapResponse response = null;
        try {
            synchronized (lock) {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId);
                String data = gson.toJson(player);
                response = client.post(data, MediaTypeRegistry.APPLICATION_JSON);
                System.out.println("Add role player: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.CREATED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean leaveGroupRole(String groupRoleId, RolePlayer.PlayerInfo player){
        CoapResponse response = null;
        try {
            synchronized (lock) {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId + "/" + player.id);
                response = client.delete();
                System.out.println("Delete role player: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.DELETED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean createGroupRole(GroupRole.GroupRoleInfo roleInfo){

        // synchronous
        CoapResponse response = null;
        try {
            synchronized (lock) {
                client.setURI("coap://localhost:5683/room1/");
                String data = gson.toJson(roleInfo);
                response = client.post(data, MediaTypeRegistry.APPLICATION_JSON);
                System.out.println("Add role player: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.CREATED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean updateGroupRole(GroupRole.GroupRoleInfo roleInfo){
        CoapResponse response = null;
        try {
            synchronized (lock) {
                client.setURI("coap://localhost:5683/room1/" + roleInfo.id);
                String data = gson.toJson(roleInfo);
                response = client.put(data, MediaTypeRegistry.APPLICATION_JSON);
                System.out.println("Update role player: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.CHANGED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static boolean deleteGroupRole(String groupRoleId){
        CoapResponse response = null;
        try {
            synchronized (lock) {
                client.setURI("coap://localhost:5683/room1/" + groupRoleId );
                response = client.delete();
                System.out.println("Delete group role: " + response.getResponseText());
                return response.getCode() == CoAP.ResponseCode.DELETED;
            }
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
