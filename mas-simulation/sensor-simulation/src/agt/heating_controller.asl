program_active(0).
!start.

+!start : true
    <- .print("Starting heating controller");
       .date(Y,M,D); .time(H,Min,Sec,MilSec); // get current date & time
       +started(Y,M,D,H,Min,Sec);
       !!start_simulator.

+!start_simulator:true
    <- .print("Starting simulator");
        simulateProgramActivity.

+sensor_data(D)[source(Ag)] :program_active(S) & S == 1
    <- processInput.

+sensor_data(D)[source(Ag)] :program_active(S) & S == 0
    <- nop.

+program_active(S):S == 1
    <- .print("State changed to ", S);
    recruitSensors.

+program_active(S):S == 0
    <-.print("State changed to ", S);
    releaseSensors.

{ include("$jacamo/templates/common-cartago.asl") }
{ include("$jacamo/templates/common-moise.asl") }

// uncomment the include below to have an agent compliant with its organisation
//{ include("$moise/asl/org-obedient.asl") }
