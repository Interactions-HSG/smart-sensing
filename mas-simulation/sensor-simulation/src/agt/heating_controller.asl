state(0).
!start.

+!start : true
    <- .print("Starting heating controller");
       .date(Y,M,D); .time(H,Min,Sec,MilSec); // get current date & time
       +started(Y,M,D,H,Min,Sec);
       !!start_scheduler.

+!start_scheduler:true
    <- .print("Starting scheduler");
        update_schedule.

+sensor_data(D)[source(Ag)] :state(S) & S > 0
    <- .print("Received usable sensor data ", D, " from ", Ag, " state ", S);
        processInput.

+sensor_data(D)[source(Ag)] :state(S) & S < 1
    <- .print("Received useless sensor data ", D, " from ", Ag).

+state(S):S > 0
    <- .print("State changed to ", S);
    activate.

+state(S):S < 1
    <-.print("State changed to ", S);
    deactivate.

{ include("$jacamo/templates/common-cartago.asl") }
{ include("$jacamo/templates/common-moise.asl") }

// uncomment the include below to have an agent compliant with its organisation
//{ include("$moise/asl/org-obedient.asl") }
