package src.node;


moduleinterface IRouting
{
    parameters:
        @display("i=block/switch");
        
    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;
}