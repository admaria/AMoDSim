import src.coordinator.ICoord;
import src.node.Node;
import src.networkmanager.INetworkManager;
import ned.DatarateChannel;

network AMoD
{
	parameters:
	    //------Network Parameters------
        int height @prompt("Number of rows") = default(5); 	//nodes on y axes
        int width @prompt("Nodes per row") = default(5); 	//nodes on x axes
        double xNodeDistance @unit(m) = default(1000m);		//Distance between nodes on X axes
        double yNodeDistance @unit(m) = default(1000m);		//Distance between nodes on Y axes
        double h @unit(m)= height * yNodeDistance + 100m; 	//simulation area: y size
        double w @unit(m) = width * xNodeDistance + 100m; 	//simulation area: x size
        
        //------Vehicle Parameters------
        double velocity @unit(mps) = default (9.7mps); 		//Vehicles velocity
        int boardingTime @unit(s) = default (0s);  			//Time needed for boarding passengers
        int alightingTime @unit(s) = default (0s); 			//Time needed for alighting passengers
        
        //------Channel Parameters------
        //--Don't touch 
        double xTravelTime @unit(s) = (dropUnit(xNodeDistance)/dropUnit(velocity))*1s; //time needed to travel a street
        double yTravelTime @unit(s) = (dropUnit(yNodeDistance)/dropUnit(velocity))*1s; //time needed to travel a street
        
        //------Coordinator type--------
        string coordType = default ("RadioTaxiCoord");
        
        //------NetworkManager type--------
        string netmanagerType = default ("AdaptiveNetworkManager");
        
        @display("bgb=$w,$h");
        
    types:
        channel xRoad extends ned.DatarateChannel
        {
            parameters:
                delay = xTravelTime; //time needed to travel the street (x axes). (propagation delay)
                double length @unit(m) = xNodeDistance;
        }
        
        channel yRoad extends ned.DatarateChannel
        {
            parameters:
                delay = yTravelTime; //time needed to travel the street (y axes). (propagation delay)
                double length @unit(m) = yNodeDistance;
        }

    submodules:
        netmanager: <netmanagerType> like INetworkManager{
            parameters:
                numberOfNodes = height * width;
        	@display("p=800,50");
        }
                
        tcoord: <coordType> like ICoord{
            @display("p=600,50");
        }
                
        n[height*width]: Node {
            parameters:
                address = index;
                x=index % width;
                y=index / height;
                x_distance = replaceUnit((index % width),"meter"); //meters from origin (x axes)
                y_distance = replaceUnit((index / height), "meter");//meters from origin (y axes)
                xBase_distance = xNodeDistance;
                yBase_distance = yNodeDistance;
                @display("p=,,m,$width,$xNodeDistance,$yNodeDistance");
            gates:
                port[5];
        }
        
        
    connections allowunconnected:
        for i=0..height-1, for j=0..width-1 {
            n[i*width+j].port[0] <--> yRoad <--> n[(i+1)*width+j].port[1] if i!=height-1;  //Vertical connections
            n[i*width+j].port[2] <--> xRoad <--> n[(i*width+j)+1].port[3] if j!=width-1;   //Horizontal connections
        }
   
}
