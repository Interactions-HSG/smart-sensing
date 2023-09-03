package controller_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;
import common.GlobalClock;
import organization_interface.FunctionalSpec;
import organization_interface.GroupRoleInfo;
import organization_interface.Organization;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;



public class ControlProgram extends Artifact implements Organization.OrganizationListener {
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

    double offeredReward = 0.0;

    String groupRoleId = "";

    void init(String type) {
        defineObsProperty("program_type", type);
        defineObsProperty("program_active", 0);

        currentState = 0;
        programType = type;
        groupRoleId = "gr_sensing_" + programType + "_" + this.getId().getName();
        loadSensorLogFile();
        addOrgGroupRole();
        GlobalClock.start();
    }

    @OPERATION
    void addOrgGroupRole() {
        FunctionalSpec fs = new FunctionalSpec();
        fs.measurementInterval = 60000;
        fs.hasQuantityKind = 1;
        fs.measurementDuration = 50;
        fs.updateInterval = 60000;
        GroupRoleInfo role = new GroupRoleInfo();
        role.id = groupRoleId;
        role.reward = 1;
        role.isActive = false;
        role.maxAgents = 5;
        role.currentAgents = 0;
        role.currentAllocation = 0;
        role.minAllocation = 20;
        role.functionalSpecification = fs;
        role.creatorId = this.getId().getName();
        Organization.createGroupRole(role);
        Organization.observeMeasurements(role.id, this);
        offeredReward = 1.0;
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
                GroupRoleInfo roleInfo = Organization.getGroupRole(groupRoleId);
                if(roleInfo != null && GlobalClock.ticks - roleInfo.isActiveSince >= roleInfo.functionalSpecification.measurementDuration){
                    System.out.println("Controller: Extending role duration");
                    roleInfo.isActive = true;
                    roleInfo.reward = 1.0f;
                    offeredReward = 1.0f;
                    roleInfo.isActiveSince = GlobalClock.ticks;
                    roleInfo.functionalSpecification.measurementDuration = 50;
                    Organization.updateGroupRole(roleInfo);
                }
                if(roleInfo != null && roleInfo.currentAllocation < 100 && cyclesActive > 3){
                    if(roleInfo.reward <= 10.0) {
                        roleInfo.reward += 0.1;
                        offeredReward += 0.1;
                        Organization.updateGroupRole(roleInfo);
                    }else{
                        System.out.printf("Controller:%s is starving\f", this.getId().getName());
                    }
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
        GroupRoleInfo roleInfo = Organization.getGroupRole(groupRoleId);
        roleInfo.isActive = true;
        roleInfo.reward = 1.0f;
        offeredReward = 1.0f;
        roleInfo.isActiveSince = GlobalClock.ticks;
        roleInfo.functionalSpecification.measurementDuration = 50;
        Organization.updateGroupRole(roleInfo);
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 1, 0, 0);
        //writeToLogFile(msg);
    }

    @OPERATION
    void releaseSensors(){

        this.log(String.format("Controller: Program end. Got %d updates in %d minutes", inputUpdates, GlobalClock.ticks - start));
        GroupRoleInfo roleInfo = Organization.getGroupRole(groupRoleId);
        roleInfo.isActive = false;
        roleInfo.reward = 0.0f;

        Organization.updateGroupRole(roleInfo);
        writeToLogFile(String.format("%d;%s;%d;%d;%d;%d;%f", GlobalClock.ticks, String.format("%d:%d", GlobalClock.hour, GlobalClock.minute), GlobalClock.ticks - start, currentState, cyclesActive, inputUpdates, offeredReward));
        offeredReward = 0.0f;
        inputUpdates = 0;
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


    @Override
    public void onGroupRoleInfoChange(String data) {

    }

    @Override
    public void onMeasurement(String groupId, String data) {
        inputUpdates++;
        System.out.printf("Controller: %s got measurement update from %s:%s\n",this.getId().getName(), groupId, data);
    }
}
