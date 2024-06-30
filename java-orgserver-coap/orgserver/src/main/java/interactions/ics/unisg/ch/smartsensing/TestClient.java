package interactions.ics.unisg.ch.smartsensing;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Date;

import com.google.gson.Gson;
import interactions.ics.unisg.ch.smartsensing.entities.*;
import org.eclipse.californium.core.*;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;

public class TestClient extends Thread {

    static long avg_response_time = 0;
    static long samples = 0;

    CoapClient client = new CoapClient("coap://10.0.1.10:5683/room1");
    public void run() {
        //System.out.println("This code is running in a thread");
        String response = null;
        try {
            Date start = new Date();
            response = client.get().getResponseText();
            Date now = new Date();
            long elapsed = now.getTime() -  start.getTime();

            avg_response_time = (avg_response_time * samples + elapsed)/(samples+1);
            samples++;
            //System.out.println("Get group info: " + response);
            System.out.println(avg_response_time);
        }catch (Exception e){}
    }

    public static void testConcurrentGets(){
        ArrayList<TestClient> clients = new ArrayList<>();
        for(int i=0; i<1; i++){
            TestClient client = new TestClient();
            clients.add(client);
        }
        for(TestClient c : clients){
            c.start();
        }
    }

    public static void addDemoPlayer(){
        CoapClient client = new CoapClient("coap://localhost:5683/room1/rl-1");
        System.out.println("Add demo player");
        Gson gson = new Gson();
        PlayerInfo pi = new PlayerInfo();
        pi.id = "sen-test";
        pi.taskAllocation = 50;
        try {
            String response = client.post(gson.toJson(pi), MediaTypeRegistry.APPLICATION_JSON).getCode().toString();
            System.out.println("Response: " + response);
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static void removeDemoPlayer(){
        CoapClient client = new CoapClient("coap://localhost:5683/room1/rl-1/sen-test");
        System.out.println("Remove demo player");
        try {
            String response = client.delete().getCode().toString();
            System.out.println("Response: " + response);
        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static void createDemoRole(){
        CoapClient client = new CoapClient("coap://localhost:5683/room1");
        System.out.println("SYNCHRONOUS");
        Gson gson = new Gson();
        // synchronous
        String response = null;
        try {
            //Query role player
            //client.setURI("coap://10.0.1.8:5683/room1/gr_comfort_sensing/ag1");
            //response = client.get().getResponseText();
            //System.out.println("Get ag1: " + response);

            response = client.get().getResponseText();
            System.out.println("Get group info: " + response);

            //Create a test GroupRole
            GroupRoleInfo grinfo = new GroupRoleInfo();
            FunctionalSpec fspec = new FunctionalSpec();
            fspec.hasQuantityKind = 0;
            fspec.measurementInterval = 1;//in minutes
            fspec.updateInterval = 5;//in minutes
            fspec.measurementDuration = 600; //in minutes
            grinfo.id = "rl-1";
            grinfo.maxAgents = 2;
            grinfo.minAllocation = 50;
            grinfo.reward = 3;
            grinfo.functionalSpecification = fspec;
            grinfo.isActive = false;
            grinfo.isActiveSince = 0;// (new Date()).getTime();
            grinfo.creatorId = "ag-1";
            response = client.post(gson.toJson(grinfo), MediaTypeRegistry.APPLICATION_JSON).getResponseText();
            System.out.println("Created new demo role: " + response);

            //Add a role player
            /*

*/
            //Query role player
            //client.setURI("coap://localhost:5683/room1/gr_comfort_sensing/ag1");
            //response = client.get().getResponseText();
            //System.out.println("Get ag1: " + response);

            //Delete agent as role player
            //response = client.delete().getCode().toString();
            //System.out.println("Delete agent in  gr_comfort_sensing: " + response);

            //Get updated info
            //client.setURI("coap://localhost:5683/room1/gr_comfort_sensing");
            //response = client.get().getResponseText();
            //System.out.println("Get gr_comfort_sensing: " + response);

            //Delete group role
            //client.setURI("coap://localhost:5683/room1/gr_comfort_sensing");
            //response = client.delete().getResponseText();
            //System.out.println("Delete gr_comfort_sensing: " + response);

        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        //CoapResponse resp2 = client.post("payload", MediaTypeRegistry.TEXT_PLAIN);
        //System.out.println("RESPONSE 2 CODE: " + resp2.getCode());

        // asynchronous

//        System.out.println("ASYNCHRONOUS (press enter to continue)");
//
//        client.get(new CoapHandler() {
//            @Override public void onLoad(CoapResponse response) {
//                String content = response.getResponseText();
//                System.out.println("RESPONSE 3: " + content);
//            }
//
//            @Override public void onError() {
//                System.err.println("FAILED");
//            }
//        });
//
//        // wait for user
//        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
//        try { br.readLine(); } catch (IOException e) { }
//
//        // observe
//
//        System.out.println("OBSERVE (press enter to exit)");
//
//        CoapObserveRelation relation = client.observe(
//                new CoapHandler() {
//                    @Override public void onLoad(CoapResponse response) {
//                        String content = response.getResponseText();
//                        System.out.println("NOTIFICATION: " + content);
//                    }
//
//                    @Override public void onError() {
//                        System.err.println("OBSERVING FAILED (press enter to exit)");
//                    }
//                });
//
//        // wait for user
//        try { br.readLine(); } catch (IOException e) { }
//
//        System.out.println("CANCELLATION");
//
//        relation.proactiveCancel();
    }
}
