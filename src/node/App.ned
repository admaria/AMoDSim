package src.node;

//
// Node's application level
//
simple App like IApp
{
    parameters:
        int address;  			//local node address
        volatile int seatsPerVehicle = default(1);
        @display("i=block/browser");
        
    gates:
        input in;
        output out;
}


