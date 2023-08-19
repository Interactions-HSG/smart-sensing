# JaCaMo-based simuator for smart sensing agents

## Overview
The JaCaMo program consists of two kind of agents: sensors and controllers. The program of a sensor is simulated by the artifact SensorSimulator while the controllers use the artifact ControlProgram to simulate the automation logic.

A sensing agent is initialized by attaching it to a unique instance of SensorSimulator artifact (see the .jcm file).

The sensing agent has two primary beliefs: its operation ```state``` and the active role it is engaged in (```engagement```). In addition it is aware of the energy (in Joules) available in its buffer and  the current input from source (e.g. PV harvesting).

The sensor wakes up periodically (```tick``` signal) and plans the power management. Similarly, it is monitoring the organization information (```revaluate``` signal) and changes cause it to replan its role play.

The SensorSimulator contains two methods of main interest: ```computeCost``` and ```computeBenefit``` which is used to evaluate the possibility to play a role.


## How to run the example
Start the java-coap-orgserver. It contains an example group (```room1```) which has two roles: ```gr_comfort_sensing``` and ```gr_safety_sensing```). These can be examined using a CoAP client (e.g., the coap-cli program). To join a role, an agent needs to POST ```PlayerInfo``` to the group-role node, and correspondingly, DELETE to exit a role.

But all this is taken care of in the Organization.java helper class in the JaCaMo program.

After starting the java-coap-orgserver (do java -jar java-coap-orgserver.java), start the JaCaMo program by doing ```gradlew run```.

The simulation program has two sensor agents - if things go well, they will join the two roles depending on how much energy they have.

The energy available to a sensor is simulated based on the data provided by Andres (data is in the /log folder).
