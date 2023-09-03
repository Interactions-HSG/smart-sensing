# Discussions

## Terminology

- Sensor node / Sensing agent: Wireless node which has sensors and is capable of autonomously evaluating its energy availability and therefore the role it can play.
- Actuator node / Actor agent: Wireless sensor node which is responsible for control function. Usually these are fully powered. They need measurement data from sensor nodes.
- Organization management node (ON): Fully-powered wireless sink nodes (gateways, routers, etc.) which contain a partial knowledge of system organization relevant to the part of the system where they are deployed.
- Battery-powered sensors (BPS): Sensor nodes which are powered by a battery
- Energy-harvested sensors (EHS): Sensor nodes which obtain power from the environment and have a small storage (like a super cap).
- Group: An organizational entity that respresents a system part where a set of functions need to be accomplished.
- Group Role: A functional role in a Group.
- Role Player: An agent which elects to fulfill a Group Role
- Functional Specification: The specification of quality of measurement that an actor node needs.
- Reward / Bid / Offered price: The offer by an actor agent to the sensor agent.
- Cost: The cost of producing the measurement (by the sensor agent) -- this normalized as fraction of energy required from the currently available energy in the buffer.
- Benefit: The difference between the offered price and the local price perceived by the sensor agent.
- Energy buffer: A small storage in EHS.
- Input energy: The ammount of energy that is currently being harvested per time unit.
- Sleepy node: A node which will go offline (and stop computing) to conserve energy.


## Key Decisions

1.	We will go for an implementation where sensing agents can adopt multiple roles (SM).

2.	A role can be fulfilled by multiple sensing agents – the actor agent specifies what is the minimum allocation that is required along with max number of agents that can share the role. The rewards are split proportionally. However, we will not use this feature for the evaluation scenario and only describe this as theoretical contribution (SM, SH). This keeps the evaluation manageable (and coherent).

3.	Once a role is published and role players are added, the implementation allows modification of specifications. We will mention this as theoretical contribution and not evaluate its effect (SM).

4.	Multiple actor agents can share a role. In such cases they can add a bonus reward. Theoretical contribution only. (SH)

5.	Any external agent (e.g. a routing agent) can add a network cost to a role player. Theoretical contribution only (SH).

6.	Open question about how can sensing agents be cognizant of long-term benefits – we will put this in discussion section (i.e. no implementation) (SM)

7.	Open question of agent reputation – we will put this in limitations (SM).


## Current state of implementation

1.	Engineering system description gets automatically translated to Moise RDF (not part of this work). This RDF is disseminated to the sink nodes in the network according to their geographical/technical system context. For example, fully-powered router node in room1 gets group description of room1 only and then sensor and actor agents can choose to “pull” sub-groups or group-role descriptions that are relevant to them. Every time something changes in system description, changes are propagated. Therefore decentralization. Everytime agents add roles these are propagated upwards (we don’t use this in our work). This work will be described as contribution which allows automated engineering. No evaluation.

2.	JaCaMo based simulator implementation is coming to an end. Each sensing agents knows energy in buffer, current input energy, and a global clock.  I will defer implementing something on Cooja (it is more closer to wsan community than aamas). We are now ready to try out the economic models.

3.	Thingy implementation is almost done: thingy has local snippet of the organization description, knows it battery level (computed from voltage), and can communicate with org management node. I think we may not have time to do full evaluation using the Thingy. We will therefore focus on showing that the model can be ported to real hardware and it works in principle. We can consider future work (maybe targeting wsan conferences) where we combine this with cooja model.

## Evaluation scenario

-	We will have three actor agents which are responsible for functions in a room – one each for temperature control (TC), lighting control (LC), safety monitoring (SM). TC and LC activate on local demand (occupancy). When TC is active, it publishes a role Rtc which is “stable”: the measurement interval and duration are known and remains same all the time it is active. LC on the other publishes a role Rlc whose functional specification can be difference every time it becomes active. Additionally, LC publishes only short-term roles (measure light level at 100ms interval for next 2 minutes) depending on its current program state. i.e. the LC is more dynamic. Finally, SM is very predictable – it publishes a role which is long term and stable (measure air quality every 10minutes for next 12 hours).

-	We will have 5 sensing agents. All of them are multi-sensors which can measure temperature, light level, and air quality. i.e. they all can potentially fulfill all roles. 3 of these are powered by energy harvesting (EH) and 2 are battery powered (BP). The three EH sensors have each different input energy profile (peaking at different times of the day, but same average input energy) and the 2 BP sensors have different battery capacity (5000mAh and 2500mAh) 

-	We will quantify two performance values: 1. Efficacy: How many of the measurements expected to be delivered in role were actually delivered., and 2. Efficiency: How much of the daily available (or quota for BP) energy was utilized by each sensor? (we need to normalize this  by how much energy was demanded by the roles).

## Ideas and Thoughts
- If energy buffer is full for (EHS), but energy input is available, then cost can be reduced further

- An actor agent may be ready to compromise with a measurement from the past (temperature right now, or upto one hour in the past). In this case, another actor can "resell" the data.

- In our case, sensing nodes are sellers of resources (measurements) which are produced using energy. The price is **endogenously** computed based on local conditions. The actor nodes are buyers which offer a price to buy without knowing the cost. The buyers have access to a pool of money. The size of this pool is based on an estimate of energy that would be available to the sensors.

## Notes from Clearwater (Market-based control)