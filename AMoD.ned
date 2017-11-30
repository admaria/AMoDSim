import src.coordinator.TripCoordinator;
import src.node.Node;


network AMoD
{
	parameters:
        double height @prompt("Number of rows") = default(5);
        double width @prompt("Nodes per row") = default(5);
        int nodeDistance @unit(m) = default(150m); //Distance between nodes
        double h @unit(m)= height*nodeDistance + 100m; //area y size
        double w @unit(m) = width*nodeDistance + 100m; //area x size
        double channelTime @unit(s) = default(1minute); //time to travel a street
        @display("bgb=$w,$h");
        
    types:
        channel C extends ned.DatarateChannel
        {
            parameters:
                delay = channelTime; //travel 1 km in 60s (propagation delay)
                datarate = default(1Gbps);
        }

    submodules:        
        tcoord: TripCoordinator{
            @display("p=600,29");
        }
                
        n[height*width]: Node {
            parameters:
                address = index;
                x_distance = replaceUnit((index % width),"meter");
                y_distance = replaceUnit((index / width), "meter");
                base_distance = nodeDistance;
                @display("p=,,m,$width,$nodeDistance,$nodeDistance");
            gates:
                port[5];
        }

		//Use for wall
        //environment: PhysicalEnvironment {
          //  @display("p=429,29");
        //}
        
        
    connections allowunconnected:
        for i=0..height-1, for j=0..width-1 {
            n[i*width+j].port[0] <--> C <--> n[(i+1)*width+j].port[1] if i!=height-1;
            n[i*width+j].port[2] <--> C <--> n[(i*width+j)+1].port[3] if j!=width-1;
        }
   
}
