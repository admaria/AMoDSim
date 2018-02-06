package src.coordinator;


simple BaseCoord like ICoord
{
    parameters:
        @display("i=block/cogwheel_s");
        
        // ------ SIGNALS -------- //
        @signal[traveledDistance](type="double");       	//Traveled distance per vehicle
        @statistic[traveledDistance](title="Traveled distance per vehicle (km)";source="traveledDistance";unit=km;record=vector,stats;interpolationmode=none);

        @signal[waitingTime](type="double");				//Pick-up waiting time per request
        @statistic[waitingTime](title="Waiting time (minute)";source="waitingTime";unit=minute;record=vector,stats;interpolationmode=none);

        @signal[actualTripTime](type="double");				//Actual time scored on vehicle (per request)
        @statistic[actualTripTime](title="Actual Trip Time (minute)";source="actualTripTime";unit=minute;record=vector,stats;interpolationmode=none);

        @signal[stretch](type="double");
        @statistic[stretch](title="Stretch";source="stretch";record=vector,stats;interpolationmode=none);

        @signal[tripDistance](type="double");				//Distance per trip
        @statistic[tripDistance](title="Trip Distance (km)";source="tripDistance";unit=km;record=vector,stats;interpolationmode=none);

        @signal[passengersOnBoard](type="double");			//Passengers on board per vehicle at snapshot time
        @statistic[passengersOnBoard](title="Passengers On Board";source="passengersOnBoard";record=vector,stats;interpolationmode=none);

        @signal[toDropoffRequests](type="double");       	//Picked-up requests not yet dropped-off (at snapshot time)
        @statistic[toDropoffRequests](title="Picked-up Requests";source="toDropoffRequests";record=vector,stats;interpolationmode=none);

        @signal[toPickupRequests](type="double");       	//Requests assigned but not yet picked-up
        @statistic[toPickupRequests](title="Requests To Pickup";source="toPickupRequests";record=vector,stats;interpolationmode=none);

        @signal[requestsAssignedPerVehicle](type="double");  //Number of requests assigned per vehicle
        @statistic[requestsAssignedPerVehicle](title="Requests assigned per vehicle";source="requestsAssignedPerVehicle";record=vector,stats;interpolationmode=none);
        
        @signal[travel*-time](type="double"); 
		@statisticTemplate[travelTime](title="Travel Time Per State (minute)";unit=minute;record=vector,stats;interpolationmode=none);
		
		@signal[totalRequestsPerTime](type="double");
		@statistic[totalRequestsPerTime](title="Total Requests Per Time";source="totalRequestsPerTime";record=vector;interpolationmode=none);
		
		@signal[assignedRequestsPerTime](type="double");
		@statistic[assignedRequestsPerTime](title="Assigned Requests Per Time";source="assignedRequestsPerTime";record=vector;interpolationmode=none);
		       
        @signal[pickedupRequestsPerTime](type="double");
		@statistic[pickedupRequestsPerTime](title="Picked-up Requests Per Time";source="pickedupRequestsPerTime";record=vector;interpolationmode=none);
        
        @signal[droppedoffRequestsPerTime](type="double");
		@statistic[droppedoffRequestsPerTime](title="Dropped-off Requests Per Time";source="droppedoffRequestsPerTime";record=vector;interpolationmode=none);
		
		@signal[freeVehiclesPerTime](type="double");
		@statistic[freeVehiclesPerTime](title="Free Vehicles Per Time";source="freeVehiclesPerTime";record=vector;interpolationmode=none);
}