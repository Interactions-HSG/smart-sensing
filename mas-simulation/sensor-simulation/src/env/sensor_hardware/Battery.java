
package sensor_hardware;

import cartago.*;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class Battery extends Artifact {
	int inputEnergy = 0;
	int energyConsumed = 0;
	int currentLevel = 100;

	void init(int initialValue) {
		defineObsProperty("bat_level", initialValue);
        defineObsProperty("input_energy", 0);
        defineObsProperty("energy_consumed", 0);
		defineObsProperty("temperature", initialValue);
		defineObsProperty("humidity", initialValue);
		defineObsProperty("light_level", initialValue);
		String wd =  System.getProperty("user.dir");
		currentLevel = initialValue * 825*3600/100;
		loadPowerLogFile();
		loadSensorLogFile();
	}

	List<List<String>> powerRecords = new ArrayList<>();
	void loadPowerLogFile(){
		String wd =  System.getProperty("user.dir");
		try (BufferedReader br = new BufferedReader(new FileReader(wd + "/log/power_agg_pos16_5min.csv"))) {
			String line;
			while ((line = br.readLine()) != null) {
				String[] values = line.split(";");
				powerRecords.add(Arrays.asList(values));
			}
		} catch (FileNotFoundException e) {
			throw new RuntimeException(e);
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
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
	@OPERATION
	void charge(int value) {
		inputEnergy = value;
	}

	@OPERATION
	void discharge(int value) {
		energyConsumed = value;
	}

	int idx = 1;
	@OPERATION
	void update_battery_state() {
        ObsProperty propL = getObsProperty("bat_level");
		ObsProperty propI = getObsProperty("input_energy");
		ObsProperty propT = getObsProperty("temperature");
        while(true){
			String pbat_str = powerRecords.get(idx).get(5);
			double pbat_mw = Double.parseDouble(pbat_str) * 1000;
			inputEnergy = (int) (pbat_mw * 5 * 60); //milli joules
			propI.updateValue(inputEnergy);
			propI.commitChanges();

			String tmp_str = sensorRecords.get(idx++).get(5);
			double temperature = Double.parseDouble(tmp_str);
			propT.updateValue(temperature);
			propT.commitChanges();

            if(energyConsumed > inputEnergy && currentLevel > 0){
				currentLevel -= (energyConsumed - inputEnergy);
				int batLevel = ((currentLevel *100)/ (825*3600));
                propL.updateValue(batLevel);
                propL.commitChanges();
            }
			//battery capacity CR2032 of 250mAh = 825mWh
            if(energyConsumed < inputEnergy && currentLevel < 825*3600){
				currentLevel += (energyConsumed - inputEnergy);
				int batLevel = ((currentLevel * 100)/ (825*3600)) ;
				propL.updateValue(batLevel);
                propL.commitChanges();
            }
			signal("tick");
            await_time(1000);
        }		
	}

	@OPERATION
	void getCharge(int inc, OpFeedbackParam<Integer> newValueArg) {
		ObsProperty prop = getObsProperty("bat_level");
		int newValue = prop.intValue()+inc;
		prop.updateValue(newValue);
		newValueArg.set(newValue);
	}

}
