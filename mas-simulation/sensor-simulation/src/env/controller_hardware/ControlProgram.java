package controller_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;
import jade.util.Logger;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ControlProgram extends Artifact {
    int inputUpdates = 0;
    long start = 0;

    int currentState = 0;
    int hour = 19;
    int minute = 20;
    int idx = 1;


    String wd =  System.getProperty("user.dir");
    String fileName = wd + "/log/runtime_con_";

    int cyclesActive = 0;
    List<List<String>> sensorRecords = new ArrayList<>();

    void init(int initialValue) {
        defineObsProperty("state", initialValue);
        currentState = initialValue;
        loadSensorLogFile();
    }

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

    @OPERATION
    void update_schedule() {
        ObsProperty propState = getObsProperty("state");
        while(true){
            if(idx > 10000){
                break;
            }
            boolean occupied = false;
            if(hour >=8 && hour <= 19){
                occupied = Math.random() > 0.3;
            }else{
                occupied = Math.random() > 0.8;
            }
            if(occupied && currentState == 0){
                currentState = 1;
                cyclesActive = 0;
                propState.updateValue(1);
            }else if (!occupied && currentState == 1 && cyclesActive > 15){
                currentState = 0;
                propState.updateValue(0);
                writeToLogFile(String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), currentState, cyclesActive, inputUpdates));
            }
            if(currentState == 1){
                cyclesActive++;
            }
            propState.commitChanges();
            signal("tick");
            minute++;
            if(minute == 60){
                minute = 0;
                hour++;
                if(hour == 24){
                    hour = 0;
                }
            }
            idx++;

            await_time(250);
        }
    }

    @OPERATION
    void activate(){
        start = System.currentTimeMillis();
        inputUpdates = 0;
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 1, 0, 0);
        //writeToLogFile(msg);
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
    @OPERATION
    void deactivate(){
        this.log(String.format("Got %d updates in %d seconds", inputUpdates, (System.currentTimeMillis() - start)/1000));
        inputUpdates = 0;
        //String msg = String.format("%s;%d;%d;%d", sensorRecords.get(idx).get(0), 0, inputUpdates, cyclesActive);
        //writeToLogFile(msg);
    }
    @OPERATION
    void processInput(){
        inputUpdates++;
    }
}
