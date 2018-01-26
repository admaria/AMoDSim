
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

        if(isRequestValid(*tr))
        {
            totrequests++;
            handleTripRequest(tr);
        }
        else
        {
            EV << "The request " << tr->getID() << " is not valid!" << endl;
        }
    }

}


void RadioTaxiCoord::handleTripRequest(TripRequest *tr)
{
    std::map<int,std::list<StopPoint*>> vehicleProposals;

    for (auto const &x : vehicles)
    {
        //Check if the vehicle has enough seats to serve the request
        if(x.first->getSeats() >= tr->getPickupSP()->getNumberOfPassengers())
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
    StopPoint *pickupSP = new StopPoint(*tr->getPickupSP());
    StopPoint *dropoffSP = new StopPoint(*tr->getDropoffSP());

    double dst_to_pickup = -1;
    double dst_to_dropoff = -1;
    std::list<StopPoint*> old = rPerVehicle[vehicleID];
    std::list<StopPoint*> newList;

    //-----The Vehicle is empty-----
    if(rPerVehicle.find(vehicleID) == rPerVehicle.end() || old.empty())
    {
        EV << "The vehicle " << vehicleID << " has not other stop points!" << endl;
        dst_to_pickup = netmanager->getTimeDistance(getLastVehicleLocation(vehicleID), pickupSP->getLocation());
        dst_to_dropoff = netmanager->getTimeDistance(pickupSP->getLocation(), dropoffSP->getLocation()) + boardingTime;

        if (dst_to_pickup >= 0 && dst_to_dropoff >= 0)
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
        dst_to_pickup = netmanager->getTimeDistance(sp->getLocation(), pickupSP->getLocation()) + (sp->getActualTime() - currentTime) + alightingTime; //The last stop point is a dropOff point.
        dst_to_dropoff = netmanager->getTimeDistance(pickupSP->getLocation(), dropoffSP->getLocation()) + boardingTime;

        if (dst_to_pickup >= 0 && dst_to_dropoff >= 0)
        {
            pickupSP->setActualTime(dst_to_pickup + currentTime);
            dropoffSP->setActualTime(pickupSP->getActualTime() + dst_to_dropoff);

            EV << "Time needed to vehicle: " << vehicleID << " to reach pickup: " << pickupSP->getLocation() << " from current time, is: " << (pickupSP->getActualTime()-currentTime)/60  << " minutes." << endl;
            EV << "Time needed to vehicle: " << vehicleID << " to reach dropoff: " << dropoffSP->getLocation() << " from current time, is: " << (dropoffSP->getActualTime()-currentTime)/60 << " minutes." << endl;
        }
    }

    if(dst_to_pickup != -1 && pickupSP->getActualTime() <= (pickupSP->getTime() + pickupSP->getMaxDelay()))// && dropoffSP->getActualTime() <= (dropoffSP->getTime() + dropoffSP->getMaxWaitingTime()))
    {
        for (auto const &x : old)
            newList.push_back(new StopPoint(*x));

        newList.push_back(pickupSP);
        newList.push_back(dropoffSP);
    }
    else{
        delete pickupSP;
        delete dropoffSP;
    }

    return newList;
}
