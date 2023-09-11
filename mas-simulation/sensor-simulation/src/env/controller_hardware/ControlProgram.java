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

    int cyclesExpected = 0;
    List<List<String>> sensorRecords = new ArrayList<>();

    double offeredReward = 0.0;

    String groupRoleId = "";

    Organization organization = new Organization();

    int time_step = 1000;

    void init(String type) {
        defineObsProperty("program_type", type);
        defineObsProperty("program_active", 0);
        currentState = 0;
        programType = type;
        groupRoleId = "gr_sensing_" + programType + "_" + this.getId().getName();
        writeToLogFile("Ticks;Time;Duration;State;Cycles;Updates;Expected;Reward");
        loadSensorLogFile();
        addOrgGroupRole();
        GlobalClock.start();
    }

    FunctionalSpec getFunctionalSpec(){
        FunctionalSpec fs = new FunctionalSpec();
        if(programType.equals("TC")) {
            fs.measurementInterval = 120000; // => 50 measurements
            fs.hasQuantityKind = 0x01; //Temperature
            fs.measurementDuration = 100; // 10 windows 5 meas per window
            fs.updateInterval = 60000;
        }else if(programType.equals("LC")){
            fs.measurementInterval = 60000; // => 50 measurements
            fs.hasQuantityKind = 0x04; //Light level
            fs.measurementDuration = 50; // 3 windows 15meas per window => 15 * 0.10 = 1.5 J.
            fs.updateInterval = 10000;
        }else if(programType.equals("SM")){
            fs.measurementInterval = 300000; // => 60 measurements
            fs.hasQuantityKind = 0x01 ; //Temperature, CO2| 0x08
            fs.measurementDuration = 300; // 60 windows, 1 meas per window =>
            fs.updateInterval = 60000;
        }
        else{
            System.out.println("Unknown control program");
        }
        return fs;
    }
    @OPERATION
    void addOrgGroupRole() {
        GroupRoleInfo role = new GroupRoleInfo();
        role.id = groupRoleId;
        role.reward = 1;
        role.isActive = false;
        role.maxAgents = 5;
        role.currentAgents = 0;
        role.currentAllocation = 0;
        role.minAllocation = 20;
        role.functionalSpecification = getFunctionalSpec();
        role.creatorId = this.getId().getName();
        organization.createGroupRole(role);
        organization.observeMeasurements(role.id, this);
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
            if(occupied && currentState == 0 /*&& cyclesInactive > 2*/){ //5min delay
                currentState = 1;
                cyclesActive = 0;
                propProgramActive.updateValue(1);
                propProgramActive.commitChanges();
            }else if (currentState == 1){ //15min delay
                if(cyclesActive > cyclesExpected) {
                    currentState = 0;
                    cyclesInactive = 0;
                    propProgramActive.updateValue(0);
                    propProgramActive.commitChanges();
                }else {
                    cyclesActive++;
                    GroupRoleInfo roleInfo = Organization.getGroupRole(groupRoleId);
                    if (roleInfo != null && roleInfo.isActive && roleInfo.currentAllocation < 100 && cyclesActive > 1) {
                        if (roleInfo.reward <= 10.0) {
                            roleInfo.reward += 0.1;
                            offeredReward += 0.1;
                            Organization.updateGroupRole(roleInfo);
                        } else {
                            System.out.printf("Controller:%s is starving\f", this.getId().getName());
                        }
                    }
                }
            }else {
                cyclesInactive++;
            }
            idx++;
            await_time(GlobalClock.simulation_time_step);
        }
    }

    @OPERATION
    void recruitSensors(){
        this.log(String.format("Controller: Program start at %d:%d", GlobalClock.hour, GlobalClock.minute));
        start = GlobalClock.ticks;
        inputUpdates = 0;
        GroupRoleInfo roleInfo = organization.getGroupRole(groupRoleId);
        roleInfo.isActive = true;
        roleInfo.reward = 1.0f;
        offeredReward = 1.0f;
        roleInfo.isActiveSince = GlobalClock.ticks;
        //roleInfo.functionalSpecification.measurementDuration = 50;
        organization.updateGroupRole(roleInfo);
        cyclesExpected = roleInfo.functionalSpecification.measurementDuration / (GlobalClock.simulation_window_size/60000);
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 1, 0, 0);
        //writeToLogFile(msg);
    }

    @OPERATION
    void releaseSensors(){
        this.log(String.format("Controller: Program end. Got %d updates in %d minutes", inputUpdates, GlobalClock.ticks - start));
        GroupRoleInfo roleInfo = organization.getGroupRole(groupRoleId);
        roleInfo.isActive = false;
        roleInfo.reward = 0.0f;
        organization.updateGroupRole(roleInfo);
        int expectedUpdates = (roleInfo.functionalSpecification.measurementDuration * 60000) / roleInfo.functionalSpecification.measurementInterval;

        writeToLogFile(String.format("%d;%s;%d;%d;%d;%d;%d;%f", GlobalClock.ticks, String.format("%d:%d", GlobalClock.hour, GlobalClock.minute), GlobalClock.ticks - start, currentState, cyclesActive, inputUpdates, expectedUpdates, offeredReward));
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

    //-------------------------------- Organization callbacks --------------------------
    @Override
    public void onGroupRoleInfoChange(String data) {

    }

    @Override
    public void onMeasurement(String groupId, String data) {
        inputUpdates++;
        //System.out.printf("Controller: %s got measurement update from %s:%s\n",this.getId().getName(), groupId, data);
    }
}
