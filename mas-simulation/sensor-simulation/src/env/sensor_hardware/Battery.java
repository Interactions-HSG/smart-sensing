
package sensor_hardware;

import cartago.*;
import organization_interface.GroupRole;
import organization_interface.Organization;

import java.io.*;
import java.util.*;

public class Battery extends Artifact {
	double inputEnergy = 0.0f;
	double energyConsumed = 0.0f;
	double batteryCharge = 600.0f;

	double energyPerMeasurement = 0.15f;

	String wd =  System.getProperty("user.dir");
	String fileName = wd + "/log/runtime_sen_";

	void init(double initialValue, double perMeasurement) {
		batteryCharge = initialValue;
		energyPerMeasurement = perMeasurement;
		defineObsProperty("bat_level", initialValue);
        defineObsProperty("input_energy", 0);
        defineObsProperty("energy_consumed", 0);
		defineObsProperty("temperature", 22.0f);
		defineObsProperty("humidity", 50.0f);
		defineObsProperty("light_level", 100.0f);
		defineObsProperty("current_benefit", 0);
		defineObsProperty("highest_benefit", 0);
		defineObsProperty("current_role", "unassigned");
		defineObsProperty("energy_per_measurement", perMeasurement);
		String wd =  System.getProperty("user.dir");

		loadPowerLogFile();
		loadSensorLogFile();
		//GroupRole.GroupRoleInfo[] roles = Organization.getGroupRoles();
		//System.out.println(roles);
	}

	double computeCost(GroupRole.GroupRoleInfo role){
		int interval = role.functionalSpecification.measurementInterval;
		int duration = role.functionalSpecification.measurementDuration;
		int numberOfMeasurements = (duration * 60000 /interval);
		double cost = numberOfMeasurements * energyPerMeasurement;
		return  cost;
	}
	double computeBenefit(GroupRole.GroupRoleInfo role){
		double cost = computeCost(role);
		double benefit = batteryCharge / cost;
		return benefit;
	}

	GroupRole.GroupRoleInfo getRole(String id, List<GroupRole.GroupRoleInfo> roles){
		for(GroupRole.GroupRoleInfo role : roles){
			if(role.id.equals(id)){
				return role;
			}
		}
		return null;
	}
	String currentRoleID = null;
	double currentRoleBenefit = 0;
	String bestRoleID = null;

	@OPERATION
	void updateRoleStatus() {
		ObsProperty propCB = getObsProperty("current_benefit");
		ObsProperty propHB = getObsProperty("highest_benefit");
		List<GroupRole.GroupRoleInfo> roles =  Organization.getGroupRoles();
		double maxBenefit = 0;
		GroupRole.GroupRoleInfo bestRole = null;
		for(GroupRole.GroupRoleInfo role : roles){
			double benefit = computeBenefit(role);
			if(benefit > maxBenefit){
				bestRole = role;
				maxBenefit = benefit;
			}
		}
		if(bestRole != null) {
			bestRoleID = bestRole.id;
		}

		if(currentRoleID != null){
			GroupRole.GroupRoleInfo currentRole = getRole(currentRoleID, roles);
			currentRoleBenefit = computeBenefit(currentRole);
		}
		propCB.updateValue(currentRoleBenefit);
		propCB.commitChanges();
		propHB.updateValue(maxBenefit);
		propHB.commitChanges();
	}

	@OPERATION
	void switchRole() {
		if(bestRoleID != null){
			currentRoleID = bestRoleID;
			ObsProperty propCR = getObsProperty("current_role");
			propCR.updateValue(currentRoleID);
			propCR.commitChanges();
		}
	}

	List<List<String>> powerRecords = new ArrayList<>();
	void loadPowerLogFile(){
		String wd =  System.getProperty("user.dir");
		String id = this.getId().toString();
		try (BufferedReader br = new BufferedReader(new FileReader(wd + String.format("/log/power_agg_%s_5min.csv", this.getId().toString())))) {
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
		try (BufferedReader br = new BufferedReader(new FileReader(wd + String.format("/log/sensor_agg_%s_5min.csv", this.getId().toString())))) {
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
	void charge(double value) {
		inputEnergy = value;
	}

	@OPERATION
	void discharge(double value) {
		energyConsumed = value;
		//writeToLogFile(String.format("%s;%d;%d", sensorRecords.get(idx).get(0), batteryCharge, value));
	}

	int idx = 1;
	@OPERATION
	void update_battery_state() {
        ObsProperty propL = getObsProperty("bat_level");
		ObsProperty propI = getObsProperty("input_energy");
		ObsProperty propT = getObsProperty("temperature");

        while(true){
			if(idx > 10000){
				break;
			}
			try {
				String pbat_str = powerRecords.get(idx).get(5);
				double pbat_w = Double.parseDouble(pbat_str);
				inputEnergy = (pbat_w * 5 * 60); //joules
				propI.updateValue(inputEnergy);
				propI.commitChanges();

				String tmp_str = sensorRecords.get(idx++).get(5);
				double temperature = Double.parseDouble(tmp_str) + Math.random();
				propT.updateValue(temperature);
				propT.commitChanges();

				if (energyConsumed > inputEnergy && batteryCharge > 0) {
					batteryCharge -= (energyConsumed - inputEnergy);
					//int batLevel = ((batteryCharge *100)/ (825*3600));
					propL.updateValue(batteryCharge);
					propL.commitChanges();
				}
				//battery capacity CR2032 of 250mAh = 825mWh = 2700J
				if (energyConsumed < inputEnergy && batteryCharge < 2700) {
					batteryCharge += (inputEnergy - energyConsumed);
					propL.updateValue(batteryCharge);
					propL.commitChanges();
				}
				writeToLogFile(String.format("%s;%f;%f;%f", sensorRecords.get(idx).get(0), inputEnergy, energyConsumed, batteryCharge));
			}catch(Exception e){
				this.log("Exception:" + e.getMessage());
			}
			signal("tick");
            await_time(250);
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

	@OPERATION
	void getCharge(int inc, OpFeedbackParam<Integer> newValueArg) {
		ObsProperty prop = getObsProperty("bat_level");
		int newValue = prop.intValue()+inc;
		prop.updateValue(newValue);
		newValueArg.set(newValue);
	}

}
