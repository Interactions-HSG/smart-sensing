package sensor_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Sensor extends Artifact {

    void init(double initialValue) {
        defineObsProperty("temperature", initialValue);
        loadSensorLogFile();
    }

    List<List<String>> sensorRecords = new ArrayList<>();
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

    int idx = 1;
    @OPERATION
    void read_sensor() {
        ObsProperty propT = getObsProperty("temperature");
        while(true){
            String tmp_str = sensorRecords.get(idx++).get(5);
            double temperature = Double.parseDouble(tmp_str);
            propT.updateValue(temperature);
            propT.commitChanges();
            //signal("tick");
            await_time(1000);
        }
    }
}
