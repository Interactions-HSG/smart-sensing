package controller_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;
import common.GlobalClock;
import jade.util.Logger;
import organization_interface.GroupRole;
import organization_interface.Organization;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;



public class ControlProgram extends Artifact {
    int inputUpdates = 0;
    long start = 0;
    int currentState = 0;
    int idx = 1;


    String wd =  System.getProperty("user.dir");
    String fileName = wd + "/log/runtime_con_";

    int cyclesActive = 0;
    int cyclesInactive = 0;
    List<List<String>> sensorRecords = new ArrayList<>();

    GroupRole.GroupRoleInfo role = new GroupRole.GroupRoleInfo();

    void init(int initialValue) {
        defineObsProperty("state", initialValue);
        defineObsProperty("room_occupied", 0);
        currentState = initialValue;
        loadSensorLogFile();
        addOrgGroupRole();
        GlobalClock.start();
    }

    @OPERATION
    void addOrgGroupRole() {
        GroupRole.FunctionalSpec fs = new GroupRole.FunctionalSpec();
        fs.measurementInterval = 60000;
        fs.hasQuantityKind = 1;
        fs.measurementDuration = 50;
        fs.updateInterval = 60000;
        role.id = "gr_thermal_sensing";
        role.reward = 1;
        role.isActive = false;
        role.maxAgents = 5;
        role.minAllocation = 20;
        role.functionalSpecification = fs;
        role.creatorId = this.getId().getName();
        Organization.createGroupRole(role);
    }

    @OPERATION
    void simulateOccupancy() {
        ObsProperty propOccupied = getObsProperty("room_occupied");
        while(true){
            if(idx > 10000){
                break;
            }
            boolean occupied = false;
            if(GlobalClock.hour >=8 && GlobalClock.hour <= 19){
                occupied = Math.random() > 0.3;
            }else{
                occupied = Math.random() > 0.8;
            }
            if(occupied && currentState == 0 && cyclesInactive > 5){ //5min delay
                currentState = 1;
                cyclesActive = 0;
                propOccupied.updateValue(1);
                propOccupied.commitChanges();
            }else if (!occupied && propOccupied.intValue() == 1 && cyclesActive > 15){ //15min delay
                currentState = 0;
                cyclesInactive = 0;
                propOccupied.updateValue(0);
                propOccupied.commitChanges();
            }
            if(currentState == 1){
                cyclesActive++;
            }else {
                cyclesInactive++;
            }
            idx++;
            await_time(1000);
        }
    }

    @OPERATION
    void activate(){
        start = System.currentTimeMillis();
        inputUpdates = 0;
        role.isActive = true;
        Organization.updateGroupRole(role);
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 1, 0, 0);
        //writeToLogFile(msg);
    }

    @OPERATION
    void deactivate(){
        this.log(String.format("Got %d updates in %d seconds", inputUpdates, (System.currentTimeMillis() - start)/1000));
        inputUpdates = 0;
        role.isActive = false;
        Organization.updateGroupRole(role);
        writeToLogFile(String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), currentState, cyclesActive, inputUpdates));
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 0, inputUpdates, cyclesActive);
        //writeToLogFile(msg);
    }
    @OPERATION
    void processInput(){
        inputUpdates++;
    }
    //------------------------------------------- Helper methods ----------------------------------

    void loadSensorLogFile(){
        String wd =  System.getProperty("user.dir");
        try (BufferedReader br = new BufferedReader(new FileReader(wd + "/log/sensor_agg_pos16_5min.csv"))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] values = line.split(";");
                sensorRecords.add(Arrays.asList(values));
            }
        } catch (FileNotFoundException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    BufferedWriter writer;
    void writeToLogFile(String msg){
        try {
            if(writer == null){
                writer = new BufferedWriter(new FileWriter(fileName + this.getId().toString() + ".csv", true));
            }
            writer.append(msg).append("\n");
            writer.flush();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }



}
