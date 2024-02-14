# Instructions to use the supplementary material

In this document we describe how our implementation on real embedded hardware and equivalent software simulations can be used.

## Implmentation of sensor nodes on real hardware

### Overview
We have implmented prototype hardware implmentations using the [Nordic Thingy53](https://www.nordicsemi.com/Products/Development-hardware/Nordic-Thingy-53).

### System topology
We use [Thread](https://www.threadgroup.org/What-is-Thread/Thread-Benefits) a 802.15.4-based standard for low-power wireless networks. In our hardware deployment, all nodes are child of the border router which runs on a Raspberrry Pi. All nodes can communicate with any other node either by unicast or mesh-local multicast.
![Deployment topology](/images/topology.png)

### Code artifacts
The projects for this can be found in `thingy-hardware` folder. To build the projects locally, the easiest way is copy the folder and import it in to Visual Studo Code with NRF Connect plug-in installed. For detailed instructions, please see [here](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/device_guides/working_with_nrf/nrf53/thingy53_gs.html).

There are three projects of relevance:

1. `thingy53-bdi-coap-client` which the implementation of our approach using embedded BDI agents. The wireless network is based on Thread and the gossiping protocol works over CoAP. You need to setup a Thread border router on which the `java-orgserver-coap` can be run and seeded with the organization description.

2. `thingy53-ble-actor` and `thingy53-ble-sensor` implements the actor and sensor nodes for the alternate approach (BLE V5 Mesh). The Mesh needs to be provisioned and we recommend using the NRF Mesh app on a smart phone. This involves settting up a group (e.g., "room-1") and the topics (e.g., "temperature", "light-level", etc.) for the sensors. The actors can subscribe to one or group-topic combination.

### Principal parts of the code
The BDI agent is programmed using the [Embedded BDI SDK](https://embedded-bdi.github.io/).

The agent program is written in AgentSpeak Language (ASL) and can be found in `src/mas-abstractions/agent/bdi-agent/agentspeak.asl`. 

The following excerpt capture a key part of the deliberation. On receiving a *gossip* fragment about the organization which informs about a new role being available, the agent examines the role description and if it has energy spare (energy in buffer - energy already committed) which can be used to fulfill the role, it evaluates the benefit (and subsequently decides to join or ignore the role)

```
+local_capabilities_changed 
    <- +evaluation_in_progress; !!evaluate_costs.
+!reevaluate
    	<- !evaluate_benefit; !decide.
+roles_available: have_spare_energy
    <- !!reevaluate.
+energy_state_changed : roles_available
    <- !!reevaluate.
+gossip_new_roles_available : have_spare_energy 
    <- +roles_available.
+joined_role
    <- !commit_energy.
+!commit_energy : have_spare_energy
    <- !udpate_energy_state.
+!commit_energy : no_spare_energy
    <- !exit_role.
```
A similar deliberation is also implemented using a *reactive* agent (src/mas-abstractions/agent/ReactiveAgent.cpp):
```cpp
void ReactiveAgent::delibrate(){
    double e_c = getEnergyCommitted();
    double e_b = getEnergyInBuffer();

    //Spare energy? Get available roles
    if(e_b - e_c > MIN_ENERGY)
    {
        std::list<GroupRoleInfo*> availableRoles = Organization::getAvailableRoles();
        for(GroupRoleInfo* gr : availableRoles)
        {
            if(calculateBenefit(gr, e_b, e_c) > 0){
                Organization::joinRole(gr->id);
                break;
            }
        }
    }
}
```

### Memory footprint
To examine the memory footprint (Flash and SRAM) for practical deployment, we recommend:
1. Enabling compiler flag ```-o3```
2. Use the `prj-minimal.conf` which disables logging

Typical values that we achieved are listed in `thingy-hardware/compiler-summary.txt`

### Energy-harvesting
The Thingy53 requires a small hardware modification to power it using an energy-harvesting source. We used a Photo-voltaic array together with a [power-management chip](https://www.ti.com/lit/ds/symlink/bq25570.pdf).

![Thingy53 powered by PV](/images/thingy-on-the-wall.png)

The three PV-powered sensors were placed in an office room (large shared space) at positions which reflected energy profiles EP1, EP2, and EP3:

![Thingy53 powered by PV](/images/thingy-positions.png)

### Power measurements
We recommend using the NRF Power Profiler to measure the energy consumption of sensors. As a convenience, we have placed the logs of the Power Profiler in the `thingy-hardware/energy-measurements` folder.

![Thingy53 power measurements](/images/thingy-on-the-desk.png)

## Software simulator - MAS
To ease the understanding of our implementation and experiment with the working of the market model etc., we have created a MAS-based simulator using the [JaCaMo framework](https://github.com/jacamo-lang/jacamo).
Running the simuator is easy: In the `mas-simulation` folder run:

```console
./gradlew run
```
The configuration of the simulator can be changed in the file  `mas-simulation/sensor-simulation/sensor_simulation.jcm`

A sensor can be configured with the following parameters: 1. The initial amount of energy in its buffer, 2. The energy required by the sensing electronics to make one single measurement, 3. The energy profile that it uses (profile1/profile2/profile3), 3. the disturbance factor to use while simulating energy input, and 3. if the sensor is statically bound to a role (like in BLE Mesh), the names of the role with ; as delimiter.

For example:

```artifact sen01: sensor_hardware.SensorSimulator(510.0, 0.15, "profile1", 0.1, "gr_sensing_TC;gr_sensing_SM")```

Defines a sensor which has 510J in buffer at start, with 0.15J required per measurement, 10% noise introduced in the input energy during simulation, and bound to two roles (role names).

The simulator logs the energy buffer states of each sensor and the summary of role fulfillment in the actor nodes. These are stored in the `mas-simulation/sensor-simulation/log` folder.

In the same folder a Python notebook (`smart_sensing_analysis.ipynb`) is made avaiable - this can be used to analyse the log files and examine the key results.

The mental state of an agent can be viewed by a web browser making an http connection to localhost:3272
![Agents](/images/mind-of-the_agent.png)

## Software simulator for Autonomous Gossiping Protocol
Our original simulator was implemented in Cooja - due to its large package size we are unable to attach it as supplementary material.
Instead, we offer a Java-based simulator using the JProwler framework from tinyOS. The simulator can be found in the `network-simulation/JProwler` folder - the program can be imported and run in any standard Java development environment.
The simulation parameters are placed as static variables in the `Launcher.java` file.
The simulator displays a graphical view -- the root node periodically sends out orgnization updates and once a sensor node recieves a fragment relevant to it, it changes it color to green.

![Thingy53 power measurements](/images/ag-simulation.png)
