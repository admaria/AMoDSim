
package src.networkmanager;

simple AdaptiveNetworkManager like INetworkManager
{
     parameters:
        int numberOfVehicles = default(1);
        int numberOfNodes;        
        bool onlineRouting = default(false);
        int numberOfFixedStops = default(0);
        string fixedStopPointsDistribution = default("Grid");    
        @display("i=block/network2_l");      
}
