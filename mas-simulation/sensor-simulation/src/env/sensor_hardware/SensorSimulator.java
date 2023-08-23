
package sensor_hardware;

import cartago.*;
import common.GlobalClock;
import organization_interface.GroupRole;
import organization_interface.Organization;
import organization_interface.RolePlayer;
import java.io.*;
import java.util.*;

public class SensorSimulator extends Artifact implements Organization.OrganizationListener {
	double energyInput = 0.0f; 		// Amount of energy obtained (from PV array) in one cycle
	double energyConsumed = 0.0f; 	// Amount of energy consumed in one cycle
	double energyInBuffer = 0.0f; 	// Amount of energy in the buffer (capacitor / battery)
	double energyPerMeasurement = 0.15f; //Energy consumed per measurement
	String workingDirectory =  System.getProperty("user.dir");
	String fileName = workingDirectory + "/log/runtime_sen_";
	String myName = "unknown"; // Id of the sensor (not the agent)
	String currentRoleID = null;
	double currentRoleBenefit = 0;
	double currentReward = 0;
	String altRoleID = null;
	double altRoleBenefit = 0;
	double altRoleReward = 0;

	int minutesInCurrentRole = 0;

	void init(double initialValue, double perMeasurement) {
		energyInBuffer = initialValue;
		energyPerMeasurement = perMeasurement;
		defineObsProperty("energy_in_buffer", initialValue);
        defineObsProperty("energy_input", 0);
        defineObsProperty("energy_consumed", 0);
		defineObsProperty("energy_per_measurement", perMeasurement);
		defineObsProperty("temperature", 22.0f);
		defineObsProperty("humidity", 50.0f);
		defineObsProperty("light_level", 100.0f);
		defineObsProperty("current_benefit", 0);
		defineObsProperty("alternative_benefit", 0);
		defineObsProperty("current_role", "");
		defineObsProperty("alternative_role", "");

		String wd =  System.getProperty("user.dir");

		loadPowerLogFile();
		loadSensorLogFile();
		myName = this.getId().toString();
	}

	//-------------------------------- Utility functions -----------------------------------
	double computeCost(GroupRole.GroupRoleInfo role){
		if(role.functionalSpecification == null){
			return 1.0f;
		}
		int interval = role.functionalSpecification.measurementInterval;
		int duration = role.functionalSpecification.measurementDuration;
		int numberOfMeasurements = (duration * 60000 /interval);
		double cost = numberOfMeasurements * energyPerMeasurement;
		return  cost;
	}
	double computeBenefit(GroupRole.GroupRoleInfo role){
        if(!role.isActive){
            return 0.0f;
        }
		double r = role.reward;
		double cost = computeCost(role);
		double usableEnergy = energyInBuffer - 500; //low limit is 500
		if(usableEnergy <= 0){
			return 0;
		}
		double benefit = r  - (cost / usableEnergy);
		return benefit;
	}

	//---------------------------------------- Artifact operations ---------------------------
	@OPERATION
	void observeOrganization() {
		while(true){
			await_time(500);
			updateCurrentRoleState();
			findAltRole();
			signal("onOrgUpdate");
			await_time(500);
		}
	}

	@OPERATION
	void updateCurrentRoleState() {
		if(currentRoleID != null && !currentRoleID.isEmpty()){
			GroupRole.GroupRoleInfo roleInfo = Organization.getGroupRole(currentRoleID);

			if(roleInfo.id.equals(currentRoleID)){
				double cost=0;
				double fraction=0;
				double usableEnergy=0;
				if(!roleInfo.isActive){
					currentRoleBenefit = 0;
				}else {
					cost = computeCost(roleInfo);
					fraction = 1.0 - ((double)minutesInCurrentRole / (double)roleInfo.functionalSpecification.measurementDuration);
					cost = cost * fraction;
					usableEnergy = energyInBuffer - 500; //low limit is 500
					if(usableEnergy <= 0 || fraction <= 0){
						System.out.printf("[%s] Updating current role %s: Moving to CB=0 fraction=%f uEnergy=%f\n", myName, currentRoleID, fraction, usableEnergy );
						currentRoleBenefit = 0;
					}else {
						currentRoleBenefit = currentReward - (cost / usableEnergy);
					}
				}
				System.out.printf("[%s] Updating current role %s: CB=%f cost=%f fraction=%f uEnergy=%f\n", myName, currentRoleID, currentRoleBenefit, cost, fraction, usableEnergy );
				ObsProperty propCB = getObsProperty("current_benefit");
				propCB.updateValue(currentRoleBenefit);
				propCB.commitChanges();
				//signal("onOrgUpdate");
			}
		}
	}

	@OPERATION
	void findAltRole() {
		ObsProperty propHB = getObsProperty("alternative_benefit");
		ObsProperty propAR = getObsProperty("alternative_role");
		List<GroupRole.GroupRoleInfo> roles =  Organization.getGroupRoles();
		double maxBenefit = 0;
		GroupRole.GroupRoleInfo bestRole = null;
		for(GroupRole.GroupRoleInfo role : roles){
			if(role.isActive) {
				double benefit = computeBenefit(role);
				if (benefit > maxBenefit) {
					bestRole = role;
					maxBenefit = benefit;
				}
			}
		}
		if(bestRole != null) {
			altRoleID = bestRole.id;
			propAR.updateValue(altRoleID);
			propAR.commitChanges();
			altRoleReward = bestRole.reward;
			altRoleBenefit = maxBenefit;
		}else {
			altRoleID = null;
			altRoleBenefit = 0;
			altRoleReward = 0;
		}
		propHB.updateValue(altRoleBenefit);
		propHB.commitChanges();
		//System.out.printf("Agent %s thinks %s (%f) role is better than %s (%f)\n", this.getId().toString(), bestRoleID, maxBenefit, currentRoleID, currentRoleBenefit);
	}

	RolePlayer.PlayerInfo meThePlayer = new RolePlayer.PlayerInfo();

	@OPERATION
	boolean exitCurrentRole() {
		boolean leaveRoleOk = false;

		if(currentRoleID == null || currentRoleID.isEmpty()){
			leaveRoleOk = true;
		}else {
			meThePlayer.id = this.getId().toString();
			System.out.printf("[%s] exiting %s (%f)\n", myName, currentRoleID, currentRoleBenefit);
			leaveRoleOk = Organization.leaveGroupRole(currentRoleID, meThePlayer);
		}
		if(!leaveRoleOk){
			System.out.printf("[%s] Could not leave role %s\n", myName, currentRoleID);
		}
		currentRoleID = null;
		currentRoleBenefit = 0;
		currentReward = 0;
		ObsProperty propCR = getObsProperty("current_role");
		propCR.updateValue(currentRoleID);
		propCR.commitChanges();
		ObsProperty propCB = getObsProperty("current_benefit");
		propCB.updateValue(currentRoleBenefit);
		propCB.commitChanges();
		return  leaveRoleOk;
	}

	void joinAlternateRole(){
		meThePlayer.id = this.getId().toString();
		meThePlayer.canAllocateUpto = 100;
		meThePlayer.taskAllocation = 100;
		meThePlayer.networkCost = 0;

		System.out.printf("[%s] joining %s (%f)\n", myName, altRoleID, altRoleBenefit);
		boolean joinOk = Organization.joinGroupRole(altRoleID, meThePlayer);

		if(joinOk) {
			currentRoleID = altRoleID;
			currentRoleBenefit = altRoleBenefit;
			currentReward = altRoleReward;
			//altRoleID = null;
			//altRoleBenefit = 0;
			//altRoleReward = 0;
			ObsProperty propCB = getObsProperty("current_benefit");
			ObsProperty propCR = getObsProperty("current_role");
			ObsProperty propAB = getObsProperty("alternative_benefit");
			ObsProperty propAR = getObsProperty("alternative_role");
			propCR.updateValue(currentRoleID);
			propCR.commitChanges();
			propCB.updateValue(currentRoleBenefit);
			propCB.commitChanges();
			//propAR.updateValue(altRoleID);
			//propAR.commitChanges();
			//propAB.updateValue(altRoleBenefit);
			//propAB.commitChanges();
			//Organization.observeGroupRole(currentRoleID, this);
		}else{
			System.out.println(System.out.printf("[%s] Could not join role %s\n", myName, altRoleID));
		}
	}
	@OPERATION
	void switchRole() {
		boolean leaveRoleOk = exitCurrentRole();
		if(!leaveRoleOk || altRoleID == null){
			return;
		}
		joinAlternateRole();
		minutesInCurrentRole = 0;
	}


	@OPERATION
	void doTask() {
		energyConsumed = energyPerMeasurement;
		//writeToLogFile(String.format("%s;%d;%d", sensorRecords.get(idx).get(0), batteryCharge, value));
	}

	@OPERATION
	void enterSleepMode() {
		energyConsumed = 0.0f; //TODO: consider sleep-mode energy consumption. Sleep is not free.
		//writeToLogFile(String.format("%s;%d;%d", sensorRecords.get(idx).get(0), batteryCharge, value));
	}

	@OPERATION
	void nop() {

	}

	double getEnergyInput1(){
		if(idx > 10000){
			idx=1;
		}
		String pbat_str = powerRecords.get(idx).get(5);
		double pbat_w = Double.parseDouble(pbat_str);
		idx++;
		return (pbat_w * 5 * 60);
	}

	double getEnergyInput2(){
		if(idx > 10000){
			idx=1;
		}
		int hour = GlobalClock.hour;
		if(hour > 18 || hour < 6){
			return 0.0;
		}
		double deg = (double) ((hour-6) * 180) /12;
		double rad = Math.toRadians(deg);
		double p = 3.0 * Math.pow(10,-4) * Math.sin(rad);
		p = p * (1 + Math.random() / 100);
		double pbat_w = Math.max(p, 0.0f);
		idx++;
		return (pbat_w * 5 * 60);
	}

	double getTemperatureMeasurement1(){
		if(idx > 10000){
			idx=1;
		}
		String tmp_str = sensorRecords.get(idx++).get(5);
		double temperature = Double.parseDouble(tmp_str) + Math.random();
		return  temperature;
	}

	int idx = 1;
	@OPERATION
	void simulateSensorState() {
        ObsProperty propL = getObsProperty("energy_in_buffer");
		ObsProperty propI = getObsProperty("energy_input");
		ObsProperty propT = getObsProperty("temperature");

        while(true){
			try {
				energyInput = getEnergyInput2(); //joules
				propI.updateValue(energyInput);
				propI.commitChanges();

				double temperature = getTemperatureMeasurement1();
				propT.updateValue(temperature);
				propT.commitChanges();

				if (energyConsumed > energyInput && energyInBuffer > 0) {
					energyInBuffer -= (energyConsumed - energyInput);
					//int batLevel = ((batteryCharge *100)/ (825*3600));
					propL.updateValue(energyInBuffer);
					propL.commitChanges();
				}
				//battery capacity CR2032 of 250mAh = 825mWh = 2700J
				if (energyConsumed < energyInput && energyInBuffer < 2700) {
					energyInBuffer += (energyInput - energyConsumed);
					propL.updateValue(energyInBuffer);
					propL.commitChanges();
				}
				System.out.println(String.format("[%s] %d:%d  EnergyInput=%f EnergyConsumed=%f EnergyStore=%f", myName, GlobalClock.hour, GlobalClock.minute, energyInput, energyConsumed, energyInBuffer));
				writeToLogFile(String.format("%s;%f;%f;%f", sensorRecords.get(idx).get(0), energyInput, energyConsumed, energyInBuffer));
			}catch(Exception e){
				this.log("Exception:" + e.getMessage());
			}
			minutesInCurrentRole+=5;
			signal("tick");
            await_time(1000);
        }		
	}

//-------------------------------------------- Helper methods --------------------------------

	GroupRole.GroupRoleInfo getRole(String id, List<GroupRole.GroupRoleInfo> roles){
		for(GroupRole.GroupRoleInfo role : roles){
			if(role.id.equals(id)){
				return role;
			}
		}
		return null;
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

	//---------------------------------- Interface implementation ---------------------------------
	@Override
	public void onGroupRoleInfoChange(String data) {
		//changed = true;
	}
}
