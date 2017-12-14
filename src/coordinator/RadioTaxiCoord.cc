
#include <RadioTaxiCoord.h>

Define_Module(RadioTaxiCoord);

double currentTime = 0.0;

void RadioTaxiCoord::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    currentTime = simTime().dbl();

    if(signalID == tripRequest)
    {
      TripRequest *tr = check_and_cast<TripRequest *>(obj);
      EV << "New TRIP request from: " << source->getFullPath() << endl;
      totrequests++;

      handleTripRequest(tr);
    }
}


void RadioTaxiCoord::handleTripRequest(TripRequest *tr)
{
    std::map<int,std::list<StopPoint*>> vehicleProposals;

    for (auto const &x : vehicles)
    {
        //Check if the vehicle has enough seats to serve the request
        if(x.first->getSeats() >= tr->getPickupSP()->getPassenger())
        {
            std::list<StopPoint *> tmp = eval_requestAssignment(x.first->getID(), tr);
            if(!tmp.empty())
                vehicleProposals[x.first->getID()] = tmp;
        }
    }

    //Assign the request to the vehicle which minimize the waiting time
    minWaitingTimeAssignment(vehicleProposals, tr);

}


/**
 * Sort the stop-points related to the specified vehicle including the new request's pickup and dropoff point, if feasible.
 * This sorting add the new Trip Request in last position.
 *
 * @param vehicleID The vehicleID
 * @param tr The new TripRequest
 *
 * @return The sorted list of stop-point related to the vehicle.
 */
std::list<StopPoint*> RadioTaxiCoord::eval_requestAssignment(int vehicleID, TripRequest* tr)
{
    TripRequest *treq = new TripRequest(*tr);
    StopPoint *pickupSP = treq->getPickupSP();
    StopPoint *dropoffSP = treq->getDropoffSP();

    std::list<StopPoint*> old = rPerVehicle[vehicleID];
    std::list<StopPoint*> newList;

    //The vehicle has not other stop point
    if(rPerVehicle.find(vehicleID) == rPerVehicle.end() || old.empty())
    {
        EV << "The vehicle " << vehicleID << " has not other stop points!" << endl;
        double dst_to_pickup = getDistance(getLastVehicleLocation(vehicleID), pickupSP->getLocation());
        double dst_to_dropoff = getDistance(pickupSP->getNodeID(), dropoffSP->getLocation()) + boardingTime;
        if (dst_to_pickup != -1)
        {
           pickupSP->setActualTime(dst_to_pickup + currentTime);
           dropoffSP->setActualTime(pickupSP->getActualTime() + dst_to_dropoff);
           EV << "Time needed to vehicle: " << vehicleID << " to reach pickup: " << pickupSP->getLocation() << " is: " << (pickupSP->getActualTime()-currentTime)/60 << " minutes." << endl;
           EV << "Time needed to vehicle: " << vehicleID << " to reach dropoff: " << dropoffSP->getLocation() << " is: " << (dropoffSP->getActualTime()-currentTime)/60 << " minutes." << endl;
        }
    }

    else
    {
        EV << "The vehicle " << vehicleID << " has other stop points!" << endl;
        //Get last stop point for the vehicle
        StopPoint *sp = old.back();
        double dst_to_pickup = getDistance(sp->getNodeID(), pickupSP->getLocation()) + (sp->getActualTime() - currentTime) + alightingTime; //The last stop point is a dropOff point.
        double dst_to_dropoff = getDistance(pickupSP->getNodeID(), dropoffSP->getLocation()) + boardingTime;

        pickupSP->setActualTime(dst_to_pickup + currentTime);
        dropoffSP->setActualTime(pickupSP->getActualTime() + dst_to_dropoff);

        EV << "Time needed to vehicle: " << vehicleID << " to reach pickup: " << pickupSP->getLocation() << " from current time, is: " << (pickupSP->getActualTime()-currentTime)/60  << " minutes." << endl;
        EV << "Time needed to vehicle: " << vehicleID << " to reach dropoff: " << dropoffSP->getLocation() << " from current time, is: " << (dropoffSP->getActualTime()-currentTime)/60 << " minutes." << endl;
    }

    if(pickupSP->getActualTime() <= (pickupSP->getTime() + pickupSP->getMaxWaitingTime()))// && dropoffSP->getActualTime() <= (dropoffSP->getTime() + dropoffSP->getMaxWaitingTime()))
    {
        for (auto const &x : old)
            newList.push_back(x);

        newList.push_back(pickupSP);
        newList.push_back(dropoffSP);
    }

    return newList;
}