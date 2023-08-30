package controller_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;
import common.GlobalClock;
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
    String programType = "TC";
    int idx = 1;
    String wd =  System.getProperty("user.dir");
    String fileName = wd + "/log/runtime_con_";
    int cyclesActive = 0;
    int cyclesInactive = 0;
    List<List<String>> sensorRecords = new ArrayList<>();

    GroupRole.GroupRoleInfo role = new GroupRole.GroupRoleInfo();

    void init(String type) {
        defineObsProperty("program_type", type);
        defineObsProperty("program_active", 0);
        currentState = 0;
        programType = type;
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
        role.id = "gr_sensing_" + programType + "_" + this.getId().getName();
        role.reward = 1;
        role.isActive = false;
        role.maxAgents = 5;
        role.minAllocation = 20;
        role.functionalSpecification = fs;
        role.creatorId = this.getId().getName();
        Organization.createGroupRole(role);
    }

    @OPERATION
    void simulateProgramActivity() {
        ObsProperty propProgramActive = getObsProperty("program_active");
        while(true){
            boolean occupied = false;
            if(GlobalClock.hour >=8 && GlobalClock.hour <= 19){
                occupied = Math.random() > 0.6;
            }else{
                occupied = Math.random() > 0.9;
            }
            if(occupied && currentState == 0 && cyclesInactive > 2){ //5min delay
                currentState = 1;
                cyclesActive = 0;
                propProgramActive.updateValue(1);
                propProgramActive.commitChanges();
            }else if (!occupied && currentState == 1 && cyclesActive > 15){ //15min delay
                currentState = 0;
                cyclesInactive = 0;
                propProgramActive.updateValue(0);
                propProgramActive.commitChanges();
            }
            if(currentState == 1){
                cyclesActive++;
                if(GlobalClock.ticks - role.isActiveSince >= role.functionalSpecification.measurementDuration){
                    System.out.println("Controller: Extending role duration");
                    role.isActive = true;
                    role.reward = 1.0f;
                    role.isActiveSince = GlobalClock.ticks;
                    role.functionalSpecification.measurementDuration = 50;
                    Organization.updateGroupRole(role);
                }

                //role.functionalSpecification.measurementDuration = 50;
                //Organization.updateGroupRole(role);
            }else {
                cyclesInactive++;
            }
            idx++;

            await_time(1000);
        }
    }

    @OPERATION
    void recruitSensors(){
        this.log(String.format("Controller: Program start at %d:%d", GlobalClock.hour, GlobalClock.minute));

        start = GlobalClock.ticks;
        inputUpdates = 0;
        role.isActive = true;
        role.reward = 1.0f;
        role.isActiveSince = GlobalClock.ticks;
        role.functionalSpecification.measurementDuration = 50;
        Organization.updateGroupRole(role);
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 1, 0, 0);
        //writeToLogFile(msg);
    }

    @OPERATION
    void releaseSensors(){
        writeToLogFile(String.format("%d;%s;%d;%d;%d;%d", GlobalClock.ticks, String.format("%d:%d", GlobalClock.hour, GlobalClock.minute), GlobalClock.ticks - start, currentState, cyclesActive, inputUpdates));

        this.log(String.format("Controller: Program end. Got %d updates in %d minutes", inputUpdates, GlobalClock.ticks - start));
        inputUpdates = 0;
        role.isActive = false;
        role.reward = 0.0f;
        Organization.updateGroupRole(role);
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 0, inputUpdates, cyclesActive);
        //writeToLogFile(msg);
    }
    @OPERATION
    void processInput(){
        inputUpdates++;
    }

    @OPERATION
    void nop() {

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
