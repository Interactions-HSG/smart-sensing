
package sensor_hardware;

import cartago.*;
import common.GlobalClock;
import organization_interface.GroupRoleInfo;
import organization_interface.Organization;
import organization_interface.PlayerInfo;

import java.io.*;
import java.util.*;

public class SensorSimulator extends Artifact implements Organization.OrganizationListener {
	double energyInput = 0.0f;         // Amount of energy obtained (from PV array) in one cycle
	double energyConsumed = 0.0f;     // Amount of energy consumed in one cycle
	double energyInBuffer = 0.0f;     // Amount of energy in the buffer (capacitor / battery)
	double energyPerMeasurement = 0.15f; //Energy consumed per measurement
	double energyAllocated = 0.0f; //Total energy that has been allocated for current roles
	double currentTotalBenefit = 0.0f;
	String workingDirectory =  System.getProperty("user.dir");
	String fileName = workingDirectory + "/log/runtime_sen_";
	String myName = "unknown"; // Id of the sensor (not the agent)

	List<PlayerInfo> currentlyPlaying = new ArrayList<>();
	List <GroupRoleInfo> alternateRoles = new ArrayList<>();

	String energyProfile = null;
	double disturbance = 0.0f;

	void init(double initialValue, double perMeasurement, String profile, double disturbance) {
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
		energyProfile = profile;
		this.disturbance = disturbance;

		if(energyProfile.startsWith("pos")) {
			loadPowerLogFile();
			loadSensorLogFile();
		}
		myName = this.getId().toString();
	}

	//-------------------------------- Utility functions -----------------------------------
	double computeCost(GroupRoleInfo role){
		int interval = role.functionalSpecification.measurementInterval; //In milliseconds
		int duration = role.functionalSpecification.measurementDuration; //In minutes
		int numberOfMeasurements = (duration * 60000 /interval);
		double fraction = (double)(duration - (GlobalClock.ticks - role.isActiveSince)) / duration;
		fraction = Math.max(fraction, 0);
		double cost = (double)numberOfMeasurements * energyPerMeasurement * fraction;
		if(energyProfile.equals("battery")){
			cost = cost * (energyInBuffer/500); //Zuschlag für nicht-Erneubaresenergie
		}
		return  cost;
	}

	double computeBenefit(GroupRoleInfo role){
		int duration = role.functionalSpecification.measurementDuration;
		double fraction = (double)(duration - (GlobalClock.ticks - role.isActiveSince)) / duration;
		fraction = Math.max(fraction, 0);
		double r= 0;
		if(role.isActive) {
			r = role.reward * fraction; //If you are joining now, you only get fraction of the reward because you cost is also a fraction
		}else{
			r = 0; //This should not be necessary as we expect the actor agent to do set the reward to zero
		}
		double cost = computeCost(role);
		//Benefit that is evaluated under current energy circumstance
		//double usableEnergy = energyInBuffer - 500 - energyAllocated; //low limit is 500
		//Alternative: Benefit evaluated under what-if we had all the available energy for alternate roles
		double usableEnergy = energyInBuffer - 500; //low limit is 500

		if(usableEnergy <= 0){
			return -101;
		}
		double benefit = r  - (cost / usableEnergy);

		if(benefit <= 0){
			System.out.println("Negative benefit");
		}
		return benefit;
	}

	//---------------------------------------- Artifact operations ---------------------------
	@OPERATION
	void observeOrganization() {
		while(true){
			await_time(500);
			//updateCurrentRoleState();
			//findAltRole();
			//refreshRoles();
			//evaluateCurrentState();
			reviewCurrentStateAndDecide();
			signal("onOrgUpdate");
			await_time(500);
		}
	}

//------------------------------------------------------------------------------------
boolean joinRole(GroupRoleInfo role, PlayerInfo player){

	boolean joinOk = Organization.joinGroupRole(role.id, player);
	return  joinOk;
}

	boolean leaveRole(PlayerInfo player){
		boolean res = Organization.leaveGroupRole(player.groupRoleId, player);
		return  res;
	}

	List<GroupRoleInfo> lastGroupRoleInfos;

	void reviewCurrentStateAndDecide(){
		List<GroupRoleInfo> groupRoles =  Organization.getGroupRoles();
		List<GroupRoleInfo> alternatives = new ArrayList<>();
		for(GroupRoleInfo groupRoleInfo : groupRoles){
			boolean expired = (GlobalClock.ticks - groupRoleInfo.isActiveSince) > groupRoleInfo.functionalSpecification.measurementDuration;
			PlayerInfo currentPlayer = isCurrentlyPlayedBy(groupRoleInfo.id);
			if(currentPlayer != null){
				//Get out of inactive or expired roles
				if(!groupRoleInfo.isActive || expired){
					leaveRole(currentPlayer);
					currentlyPlaying.remove(currentPlayer);
				}
			}else{
				double benefit = computeBenefit(groupRoleInfo);
				if(groupRoleInfo.isActive && !expired && benefit > 0) {
					groupRoleInfo.foreseenBenefit = benefit;
					alternatives.add(groupRoleInfo);
				}
			}
		}

		if(alternatives.size() == 0){
			updateCurrentBeliefs();
			return;
		}
		currentlyPlaying.sort(Comparator.comparing(PlayerInfo::getBenefit)); //ascending
		alternatives.sort(Comparator.comparing(GroupRoleInfo::getForeseenBenefit).reversed()); //descending
		//----- simple logic: leave the lowest benefit role and join the highest benefit alternative.
		PlayerInfo player = new PlayerInfo();
		player.id = myName;
		player.benefit = alternatives.get(0).foreseenBenefit;
		player.cost = computeCost(alternatives.get(0));
		player.groupRoleId = alternatives.get(0).id;
		player.startTime = GlobalClock.ticks;
		player.taskAllocation = 100;
		player.canAllocateUpto = 100;
		player.networkCost = 0;
		player.reward = alternatives.get(0).reward;

		boolean joinOk = false;
		//Leave the least beneficial role in the currently played roles, and adopt the most beneficial role in the alternate list
		if(currentlyPlaying.size() > 0 && (currentlyPlaying.get(0).benefit < alternatives.get(0).foreseenBenefit)){
			if(leaveRole(currentlyPlaying.get(0))) {
				joinOk = joinRole(alternatives.get(0), player);
				currentlyPlaying.remove(currentlyPlaying.get(0));
			}
		} else if(currentlyPlaying.size() == 0 ){
			joinOk = joinRole(alternatives.get(0), player);
		}else{
			double currentTotalCost = getCurrentTotalCost();
			for(GroupRoleInfo role : alternatives){
				double cost = computeCost(role);
				if((cost + currentTotalCost) < (energyInBuffer-500)){
					joinOk = joinRole(role, player);
					break;
				}
			}
		}
		if(joinOk){
			currentlyPlaying.add(player);
		}
		updateCurrentBeliefs();
	}

	double getCurrentTotalCost(){
		double cost = 0;
		for(PlayerInfo player:currentlyPlaying){
			cost+= player.cost;
		}
		return cost;
	}

	@OPERATION
	void refreshRoles(){
		lastGroupRoleInfos = Organization.getGroupRoles();
		if(lastGroupRoleInfos == null){
			//TODO: Quit current players
			return;
		}
		energyAllocated = 0;
		for(PlayerInfo player: currentlyPlaying){
			energyAllocated += player.cost;
		}
		System.out.printf("Sensor: energyAllocation=%f\n", energyAllocated);

		for(GroupRoleInfo roleInfo : lastGroupRoleInfos) {
			PlayerInfo player = isCurrentlyPlayedBy(roleInfo.id);
			if (player != null) {
				double c = computeCost(roleInfo);
				double b = computeBenefit(roleInfo);
				player.cost = c;
				player.benefit = b;
				if(b < 0)
					player.cost = 0;
				System.out.printf("Sensor: current role in %s is updated with cost=%f and benefit=%f\n", player.groupRoleId, player.cost, player.benefit);
			}
		}
		//Recompute how much energy is commited
		energyAllocated = 0;
		for(PlayerInfo player: currentlyPlaying){
			energyAllocated += player.cost;
		}
		System.out.printf("Sensor: After current role update, energyAllocation=%f\n", energyAllocated);

		currentlyPlaying.sort(Comparator.comparing(PlayerInfo::getBenefit));
		alternateRoles.clear();
		for(GroupRoleInfo roleInfo : lastGroupRoleInfos){
			if(isCurrentlyPlayedBy(roleInfo.id) != null){
				continue;
			}
			boolean expired = (GlobalClock.ticks - roleInfo.isActiveSince) > roleInfo.functionalSpecification.measurementDuration;
			if(!roleInfo.isActive || expired){
				continue;
			}

			double benefitOfAltRole = computeBenefit(roleInfo);
			if(benefitOfAltRole <= 0){
				continue;
			}
			double currentLowestBenefit =currentlyPlaying.size() > 0 ? currentlyPlaying.get(0).benefit : 0;
			boolean noCurrentRoles = currentlyPlaying.size() == 0;
			if(noCurrentRoles  || (benefitOfAltRole > currentLowestBenefit)) { // benefitOfAltRole is more than the lowest current.
				roleInfo.foreseenBenefit = benefitOfAltRole;
				alternateRoles.add(roleInfo);
			}
		}
	}

	@OPERATION
	void evaluateCurrentState(){
		currentlyPlaying.sort(Comparator.comparing(PlayerInfo::getBenefit)); //ascending
		alternateRoles.sort(Comparator.comparing(GroupRoleInfo::getForeseenBenefit).reversed()); //descending
		//we switch one role at a time

		if(alternateRoles.size() == 0){
			updateCurrentBeliefs();
			return;
		}

		//----- simple logic: leave the lowest benefit role and join the highest benefit alternative.
		PlayerInfo player = new PlayerInfo();
		player.id = myName;
		player.benefit = alternateRoles.get(0).foreseenBenefit;
		player.cost = computeCost(alternateRoles.get(0));
		player.groupRoleId = alternateRoles.get(0).id;
		player.startTime = GlobalClock.ticks;
		player.taskAllocation = 100;
		player.canAllocateUpto = 100;
		player.networkCost = 0;
		player.reward = alternateRoles.get(0).reward;

		boolean joinOk = false;
		//Leave the least beneficial role in the currently played roles, and adopt the most beneficial role in the alternate list
		if(currentlyPlaying.size() > 0 && (currentlyPlaying.get(0).benefit < alternateRoles.get(0).foreseenBenefit && alternateRoles.get(0).foreseenBenefit > 0)){
			if(leaveRole(currentlyPlaying.get(0))) {
				joinOk = joinRole(alternateRoles.get(0), player);
				currentlyPlaying.remove(currentlyPlaying.get(0));
			}
		} else if(currentlyPlaying.size() == 0 && alternateRoles.get(0).foreseenBenefit > 0){
			joinOk = joinRole(alternateRoles.get(0), player);
		}
		if(joinOk){
			currentlyPlaying.add(player);
		}
		updateCurrentBeliefs();
	}

	void updateCurrentBeliefs(){
		ObsProperty propCB = getObsProperty("current_benefit");
		double totalBenefit = 0;
		for(PlayerInfo p: currentlyPlaying){
			totalBenefit += p.benefit;
		}
		propCB.updateValue(totalBenefit);
		propCB.commitChanges();
		currentTotalBenefit = totalBenefit;

		energyAllocated = 0;

		for(PlayerInfo p: currentlyPlaying){
			energyAllocated += p.cost;
		}
	}

	PlayerInfo isCurrentlyPlayedBy(String groupId){
		for(PlayerInfo pi : currentlyPlaying){
			if(pi.groupRoleId.equals(groupId)){
				return pi;
			}
		}
		return null;
	}
//-------------------------------------------------------------------------------------------
@OPERATION
void doTask() {
	energyConsumed = energyPerMeasurement;
	for(PlayerInfo roleInfo : currentlyPlaying) {
		Organization.sendMeasurement(roleInfo.groupRoleId, getTemperatureMeasurement());
	}
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

	//------------------------------------ Simulation ----------------------------
	double getEnergyInput1(){
		if(idx > 10000){
			idx=1;
		}
		String pbat_str = powerRecords.get(idx).get(5);
		double pbat_w = Double.parseDouble(pbat_str);
		idx++;
		return (pbat_w * 5 * 60);
	}

	//-------------------00    01    02    03   04     05    06    07    08    09    10    11    12    13    14    15    16   17     18    19    20    21    22    23
	double[] profile1 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.2f, 0.4f, 0.6f, 0.9f, 1.2f, 1.5f, 1.2f, 0.9f, 0.6f, 0.4f, 0.2f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	double[] profile2 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.4f, 0.8f, 1.2f, 1.3f, 1.3f, 1.0f, 0.6f, 0.4f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	double[] profile3 = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.2f, 0.3f, 0.3f, 0.4f, 0.4f, 0.6f, 0.8f, 0.9f, 1.5f, 1.4f, 1.2f, 0.9f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f};

	double getEnergyInput2(){
		double e_in = 0.0;
		int hour = GlobalClock.hour;
		int hour_n = hour == 23 ? 0 : hour + 1;
		if(energyProfile.equals("sine")) {

			if (hour > 18 || hour < 6) {
				return 0.0;
			}
			double deg = (double) ((hour - 6) * 180) / 12;
			double rad = Math.toRadians(deg);
			double p = 3.0 * Math.pow(10, -4) * Math.sin(rad);
			p = p * (1 + Math.random() / 100);
			e_in = Math.max(p, 0.0f);
		} else if (energyProfile.equals("profile1")){
			e_in = profile1[hour] + (((profile1[hour_n] - profile1[hour]) * GlobalClock.minute)/60) + (profile1[hour] * Math.random() * disturbance);
		}else if (energyProfile.equals("profile2")){
			e_in = profile2[hour] + (((profile2[hour_n] - profile2[hour]) * GlobalClock.minute)/60) + (profile2[hour] * Math.random() * disturbance);
		}else if (energyProfile.equals("profile3")){
			e_in = profile3[hour] + (((profile3[hour_n] - profile3[hour]) * GlobalClock.minute)/60) + (profile3[hour] * Math.random() * disturbance);
		}
		return (e_in * Math.pow(10,-3) * 5 * 60);
	}

	double getTemperatureMeasurement(){
		double temperature = 23.0f + Math.random();
		return  temperature;
	}

	int idx = 1;

	@OPERATION
	void simulateSensorState() {
        ObsProperty propL = getObsProperty("energy_in_buffer");
		ObsProperty propI = getObsProperty("energy_input");
		ObsProperty propT = getObsProperty("temperature");
		writeToLogFile("ticks;energyInput;energyConsumed;energyInBuffer;energyAllocated;currentlyPlaying;currentTotalBenefit");
        while(true){
			try {
				energyInput = getEnergyInput2(); //joules
				propI.updateValue(energyInput);
				propI.commitChanges();

				double temperature = getTemperatureMeasurement();
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
				writeToLogFile(String.format("%d;%f;%f;%f;%f;%d;%f;%s", GlobalClock.ticks, energyInput, energyConsumed, energyInBuffer, energyAllocated,currentlyPlaying.size(), currentTotalBenefit, getRoleNames()));
			}catch(Exception e){
				this.log("Exception:" + e.getMessage());
			}
			signal("tick");
            await_time(1000);
        }
	}

//-------------------------------------------- Helper methods --------------------------------

	String getRoleNames(){
		String roles = "";
		for(PlayerInfo player:currentlyPlaying){
			roles+= player.groupRoleId + ",";
		}
		return roles;
	}
	GroupRoleInfo getRole(String id, List<GroupRoleInfo> roles){
		for(GroupRoleInfo role : roles){
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
		try (BufferedReader br = new BufferedReader(new FileReader(wd + String.format("/log/power_agg_%s_5min.csv", energyProfile)))) {
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
		try (BufferedReader br = new BufferedReader(new FileReader(wd + String.format("/log/sensor_agg_%s_5min.csv", energyProfile)))) {
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

	@Override
	public void onMeasurement(String groupId, String data) {

	}
}
