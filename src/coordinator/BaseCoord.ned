package src.coordinator;


simple BaseCoord like ICoord
{
    parameters:
        @display("i=block/cogwheel_s");
        @signal[waitingTime](type="double");
        @signal[actualTripTime](type="double");
        @signal[tripEfficiencyRatio](type="double");
        @signal[tripDistance](type="double");
        @statistic[waitingTime](title="Waiting time";unit=minute;record=vector,mean,max;interpolationmode=none);
        @statistic[actualTripTime](title="Actual Trip Time";unit=minute;record=vector,mean,max;interpolationmode=none);
        @statistic[tripEfficiencyRatio](title="Trip Efficiency Ratio";record=vector,mean,max;interpolationmode=none);
        @statistic[tripDistance](title="Trip Distance";unit=km;record=vector,mean,max;interpolationmode=none);
        
}