package src.node;

//
// Interface for network node's application level
//
moduleinterface IApp
{
    parameters:
        int address;  // local node address
        @display("i=block/browser");
    gates:
        input in;
        output out;
}


