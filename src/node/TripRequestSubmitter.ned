
package src.node;

simple TripRequestSubmitter
{
    parameters:
        int address;
        int destAddresses;  // destination addresses range
        double maxSubmissionTime @unit(s) = default(1h); //Max request submission time
        double maxDelay @unit(s) = default(2minute); //max allowed delay from pickup/dropoff request
        volatile double sendIaTime @unit(s)= default(exponential(2minute)); // time between generating packets
        volatile int passengersPerRequest = default(1);
        @display("i=block/user");

}
