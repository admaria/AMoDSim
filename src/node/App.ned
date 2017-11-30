package src.node;

//
// Node's application level
//
simple App like IApp
{
    parameters:
        int address;  // local node address
        int destAddresses;  // destination addresses range
        bool hasVehicle;
        volatile double maxWaitingTime @unit(s) = default(2minute);
        volatile double sendIaTime @unit(s)= default(1minute); // time between generating packets
        volatile int packetLength @unit(byte);  // length of one message
        @display("i=block/browser");
        
    gates:
        input in;
        output out;
}


