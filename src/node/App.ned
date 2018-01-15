package src.node;

//
// Node's application level
//
simple App like IApp
{
    parameters:
        int address;  			//local node address
        int numberOfVehicles = default(0);	//vehicles in local node at simulation start
        volatile int seatsPerVehicle = default(1);
        @display("i=block/browser");
        
    gates:
        input in;
        output out;
}


