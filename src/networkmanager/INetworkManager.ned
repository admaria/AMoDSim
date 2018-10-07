package src.networkmanager;

moduleinterface INetworkManager
{
    parameters:
        int numberOfVehicles;
        int numberOfNodes;
        int numberOfFixedStops;
        string fixedStopPointsDistribution;
		@display("i=block/network2_l");    
}