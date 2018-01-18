package src.coordinator;


simple BaseCoord like ICoord
{
    parameters:
        @display("i=block/cogwheel_s");
        @signal[waitingTime](type="double");
        @signal[actualTripTime](type="double");
        @signal[tripEfficiencyRatio](type="double");
        @signal[tripDistance](type="double");
        @statistic[waitingTime](title="Waiting time";unit=minute;record=vector,stats;interpolationmode=none);
        @statistic[actualTripTime](title="Actual Trip Time";unit=minute;record=vector,stats;interpolationmode=none);
        @statistic[tripEfficiencyRatio](title="Trip Efficiency Ratio";record=vector,stats;interpolationmode=none);
        @statistic[tripDistance](title="Trip Distance";unit=km;record=vector,stats;interpolationmode=none);
        
}