
package src.node;

simple TripRequestSubmitter
{
    parameters:
        int address;
        int destAddresses;  // destination addresses range
		double minTripLength @unit(m) = default(1000m);
        double maxSubmissionTime @unit(s) = default(-1s); //Max request submission time
        double maxDelay @unit(s) = default(2minute); //max allowed delay from pickup/dropoff request
        volatile double sendIaTime @unit(s)= default(exponential(2minute)); // time between generating packets
        volatile int passengersPerRequest = default(1);
        @display("i=block/user");

}
