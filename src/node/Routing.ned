package src.node;

//
// Models a router.
//
simple Routing
{
    parameters:
        @display("i=block/switch");
        
    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;
}


