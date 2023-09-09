
package sensor_hardware;

import cartago.*;
import common.GlobalClock;
import organization_interface.*;

import java.io.*;
import java.util.*;
import java.util.concurrent.Semaphore;

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

	final Object orgListener = new Object();
	boolean changed = false;

	Organization organization = new Organization();


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
		organization.observeGroupRole("room1", this);
	}

	//-------------------------------- Utility functions -----------------------------------
	double getCostOfMeasurement(GroupRoleInfo role){
		int q = role.functionalSpecification.hasQuantityKind;
		//------Temperature ------- Humidity ----------- Light ------------- CO2 -----------
		return ((q & 0x01)*0.030) + (((q & 0x02) >> 1)*0.050) + (((q & 0x04) >> 2) *0.10) + (((q & 0x08)>>3)*0.20);
	}

	double computeCost(GroupRoleInfo role){
		int interval = role.functionalSpecification.measurementInterval; //In milliseconds
		int duration = role.functionalSpecification.measurementDuration; //In minutes
		int numberOfMeasurements = (duration * 60000 /interval);
		double fraction = (double)(duration - (GlobalClock.ticks - role.isActiveSince)) / duration;
		fraction = Math.max(fraction, 0);
		energyPerMeasurement = getCostOfMeasurement(role);
		double cost = (double)numberOfMeasurements * energyPerMeasurement * fraction;
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
		double benefit = 0.0f;
		if(energyProfile.equals("battery") ){
			if(r > 1.1)
				benefit = r - (cost * 1.5) / 500.0f;
			else
				benefit = 0;
		}else {
			benefit = r - (cost / usableEnergy);
		}

		//if(benefit <= 0){
		//	System.out.println("Negative benefit");
		//}
		return benefit;
	}

	//---------------------------------------- Artifact operations ---------------------------
	@OPERATION
	void observeOrganization() {
		while(true){
			await_time(1000);
			{
				//updateCurrentRoleState();
				//findAltRole();
				//refreshRoles();
				//evaluateCurrentState();
				if(true) {
					reviewCurrentStateAndDecide();
					//signal("onOrgUpdate");
					changed = false;
				}
			}
		}
	}

//------------------------------------------------------------------------------------
boolean joinRole(GroupRoleInfo role, PlayerInfo player){

	boolean joinOk = organization.joinGroupRole(role.id, player);
	return  joinOk;
}

	boolean leaveRole(PlayerInfo player){
		boolean res = organization.leaveGroupRole(player.groupRoleId, player);
		return  res;
	}

	List<GroupRoleInfo> lastGroupRoleInfos;

	void reviewCurrentStateAndDecide(){

		List<GroupRoleInfo> groupRoles =  organization.getGroupRoles();
		if(groupRoles == null){
			System.out.println("No roles visible in org node");
			return;
		}

		List<GroupRoleInfo> alternatives = new ArrayList<>();
		for(GroupRoleInfo groupRoleInfo : groupRoles){
			if(groupRoleInfo.functionalSpecification == null){
				continue;
			}
			boolean roleExpired = (GlobalClock.ticks - groupRoleInfo.isActiveSince) > groupRoleInfo.functionalSpecification.measurementDuration;

			PlayerInfo currentPlayer = isCurrentlyPlayedBy(groupRoleInfo.id);
			if(currentPlayer != null){
				//Get out of inactive or expired roles
				boolean rolePlayExpired = (GlobalClock.ticks - currentPlayer.startTime) > groupRoleInfo.functionalSpecification.measurementDuration;
				if(!groupRoleInfo.isActive || roleExpired || rolePlayExpired){
					leaveRole(currentPlayer);
					currentlyPlaying.remove(currentPlayer);
					System.out.printf("Sensor %s: Leaving role %s. IsActive=%b role expired=%b committment expired=%b\n", myName, currentPlayer.groupRoleId, groupRoleInfo.isActive, roleExpired, rolePlayExpired);
				}

				if(groupRoleInfo.isActive && !roleExpired){
					double benefit = computeBenefit(groupRoleInfo);
					if(benefit > 0) {
						groupRoleInfo.foreseenBenefit = benefit;
						alternatives.add(groupRoleInfo);
					}
				}
			}else{
				double benefit = computeBenefit(groupRoleInfo);
				if(groupRoleInfo.isActive && !roleExpired && groupRoleInfo.currentAllocation < 100) {
					if(benefit > 0) {
						groupRoleInfo.foreseenBenefit = benefit;
						alternatives.add(groupRoleInfo);
					}else{
						double cost = computeCost(groupRoleInfo);
						System.out.printf("Sensor %s:Ignoring role %s because benefit=%f. Cost=%f storage=%f reward=%f \n", myName, groupRoleInfo.id, benefit, cost, energyInBuffer, groupRoleInfo.reward);
					}

				}
			}
		}

		if(alternatives.size() == 0){
			updateCurrentBeliefs();
			return;
		}
		currentlyPlaying.sort(Comparator.comparing(PlayerInfo::getBenefit)); //ascending
		alternatives.sort(Comparator.comparing(GroupRoleInfo::getForeseenBenefit).reversed()); //descending
		//System.out.println("------------- " + myName + " -------------------");
		//System.out.println("Role\tCost\tBenefit\tTime\n");
		//writeToTraceFile(String.format("%d:%d: Currently", GlobalClock.hour, GlobalClock.minute));
		//for(PlayerInfo player: currentlyPlaying){
		//	writeToTraceFile(String.format("%s\t%f\t%f\t%d\n", player.groupRoleId, player.cost, player.benefit, (GlobalClock.ticks - player.startTime)));
		//}
		//System.out.println("Alternatives:");
		//for(GroupRoleInfo gr: alternatives){
		//	writeToTraceFile(String.format("%s\t%f\t%f\t%d\n", gr.id, gr.reward, gr.foreseenBenefit, (GlobalClock.ticks - gr.isActiveSince)));
		//}
		//System.out.println("-------------------------------------");

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
		if(currentlyPlaying.size() > 0){
			//int im = Math.min(currentlyPlaying.size(), alternatives.size());
			int i = 0;
			{
				if(currentlyPlaying.get(i).benefit < alternatives.get(i).foreseenBenefit){
					System.out.printf("Sensor %s: Leaving role %s and joining %s\n", myName, currentlyPlaying.get(i).groupRoleId, alternatives.get(i).id);
					joinOk = joinRole(alternatives.get(i), player);

					System.out.printf("Sensor %s: Joining role %s resulted %b \n", myName,alternatives.get(i).id, joinOk);
					if(joinOk && leaveRole(currentlyPlaying.get(i))) {
						currentlyPlaying.remove(i);
					}
				}
			}
		}else if(currentlyPlaying.size() == 0 ){
			{
				if( (energyProfile.equals("battery") && alternatives.get(0).foreseenBenefit > 0.8) || (!energyProfile.equals("battery") && alternatives.get(0).foreseenBenefit > 0)){
					System.out.printf("Sensor %s: Joining role %s with foreseen benefit of %f  \n", myName,alternatives.get(0).id, alternatives.get(0).foreseenBenefit);
					joinOk = joinRole(alternatives.get(0), player);
					System.out.printf("Sensor %s: Joining role %s resulted %b \n", myName,alternatives.get(0).id, joinOk);
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
	//Note: Timing Sensitive
	energyConsumed = 0;
	for(PlayerInfo player : currentlyPlaying) {
		GroupRoleInfo roleInfo = Organization.getGroupRole(player.groupRoleId);
		boolean expired = roleInfo.functionalSpecification.measurementDuration <= (GlobalClock.ticks - roleInfo.isActiveSince);
		if(expired || !roleInfo.isActive){
			System.out.println("Achtung!");
			continue;
		}
		int measurementsToSend = GlobalClock.simulation_window_size / roleInfo.functionalSpecification.measurementInterval;
		double measurementEnergy = getCostOfMeasurement(roleInfo);
		for(int i=0; i < measurementsToSend; i++)
			Organization.sendMeasurement(player.groupRoleId, getTemperatureMeasurement());
		energyConsumed += (measurementsToSend * measurementEnergy);
		if(energyConsumed > 5){
			System.out.println("Stop...");
		}
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
		//Note: Timing Sensitive
		return (e_in * Math.pow(10,-3) * GlobalClock.simulation_window_size/1000);
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
				//System.out.println(String.format("[%s] %d:%d  EnergyInput=%f EnergyConsumed=%f EnergyStore=%f", myName, GlobalClock.hour, GlobalClock.minute, energyInput, energyConsumed, energyInBuffer));
				writeToLogFile(String.format("%d;%f;%f;%f;%f;%d;%f;%s", GlobalClock.ticks, energyInput, energyConsumed, energyInBuffer, energyAllocated,currentlyPlaying.size(), currentTotalBenefit, getRoleNames()));
			}catch(Exception e){
				this.log("Exception:" + e.getMessage());
			}
			signal("tick");
			//Note: Timing Sensitive
            await_time(GlobalClock.simulation_time_step);
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

	void writeToTraceFile(String msg){
		try {
			if(writer == null){
				writer = new BufferedWriter(new FileWriter(fileName + myName + "_trace.txt", true));
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
		{
			changed = true;
		}
	}

	@Override
	public void onMeasurement(String groupId, String data) {

	}
}
