package src.coordinator;

simple TripCoordinator
{
    parameters:
        @display("i=block/cogwheel_s");
        @signal[waitingTime](type="double");
        @statistic[waitingTime](title="Waiting time";unit=s;record=vector,mean,max;interpolationmode=none);
}
