/*
########################################################
##           __  __       _____   _____ _             ##
##     /\   |  \/  |     |  __ \ / ____(_)            ##
##    /  \  | \  / | ___ | |  | | (___  _ _ __ ___    ##
##   / /\ \ | |\/| |/ _ \| |  | |\___ \| | '_ ` _ \   ##
##  / ____ \| |  | | (_) | |__| |____) | | | | | | |  ##
## /_/    \_\_|  |_|\___/|_____/|_____/|_|_| |_| |_|  ##
##                                                    ##
## Author:                                            ##
##    Andrea Di Maria                                 ##
##    <andrea.dimaria90@gmail.com>                    ##
########################################################
*/

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
            pendingRequests.insert(std::make_pair(tr->getID(),  new TripRequest(*tr)));
            totrequests++;
            emit(totalRequestsPerTime, totrequests);
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
    std::map<int,StopPointOrderingProposal*> vehicleProposals;

    for (auto const &x : vehicles)
    {
        //Check if the vehicle has enough seats to serve the request
        if(x.first->getSeats() >= tr->getPickupSP()->getNumberOfPassengers())
        {
            StopPointOrderingProposal *tmp = eval_requestAssignment(x.first->getID(), tr);
            if(tmp)
                vehicleProposals[x.first->getID()] = tmp;
        }
    }

    //Assign the request to the vehicle which minimize the waiting time
    if(requestAssignmentStrategy == 0)
        minCostAssignment(vehicleProposals, tr);
    else
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
StopPointOrderingProposal* RadioTaxiCoord::eval_requestAssignment(int vehicleID, TripRequest* tr)
{
    StopPoint *pickupSP = new StopPoint(*tr->getPickupSP());
    StopPoint *dropoffSP = new StopPoint(*tr->getDropoffSP());
    dropoffSP->setNumberOfPassengers(-pickupSP->getNumberOfPassengers());

    double dst_to_pickup = -1;
    double dst_to_dropoff = -1;
    double additionalCost = -1;
    std::list<StopPoint*> old = rPerVehicle[vehicleID];
    std::list<StopPoint*> newList;
    StopPointOrderingProposal *proposal = NULL;

    //-----The Vehicle is empty-----
    if(rPerVehicle.find(vehicleID) == rPerVehicle.end() || old.empty())
    {
        EV << "The vehicle " << vehicleID << " has not other stop points!" << endl;
        dst_to_pickup = netmanager->getTimeDistance(getLastVehicleLocation(vehicleID), pickupSP->getLocation());
        dst_to_dropoff = netmanager->getTimeDistance(pickupSP->getLocation(), dropoffSP->getLocation()) + (boardingTime*abs(pickupSP->getNumberOfPassengers()));
        additionalCost = dst_to_pickup + dst_to_dropoff;

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
        additionalCost = netmanager->getTimeDistance(sp->getLocation(), pickupSP->getLocation());
        dst_to_pickup = additionalCost + (sp->getActualTime() - currentTime) + (alightingTime*abs(sp->getNumberOfPassengers())); //The last stop point is a dropOff point.
        dst_to_dropoff = netmanager->getTimeDistance(pickupSP->getLocation(), dropoffSP->getLocation()) + (boardingTime*pickupSP->getNumberOfPassengers());
        additionalCost += dst_to_dropoff;

        if (dst_to_pickup >= 0 && dst_to_dropoff >= 0)
        {
            pickupSP->setActualTime(dst_to_pickup + currentTime);
            dropoffSP->setActualTime(pickupSP->getActualTime() + dst_to_dropoff);

            EV << "Time needed to vehicle: " << vehicleID << " to reach pickup: " << pickupSP->getLocation() << " from current time, is: " << (pickupSP->getActualTime()-currentTime)/60  << " minutes." << endl;
            EV << "Time needed to vehicle: " << vehicleID << " to reach dropoff: " << dropoffSP->getLocation() << " from current time, is: " << (dropoffSP->getActualTime()-currentTime)/60 << " minutes." << endl;
        }
    }

    //The vehicle can satisfy the request within its deadline
    if(dst_to_pickup != -1 && pickupSP->getActualTime() <= (pickupSP->getTime() + pickupSP->getMaxDelay()))// && dropoffSP->getActualTime() <= (dropoffSP->getTime() + dropoffSP->getMaxWaitingTime()))
    {
        for (auto const &x : old)
            newList.push_back(new StopPoint(*x));

        pickupSP->setActualNumberOfPassengers(pickupSP->getNumberOfPassengers());
        dropoffSP->setActualNumberOfPassengers(0);
        newList.push_back(pickupSP);
        newList.push_back(dropoffSP);

        proposal = new StopPointOrderingProposal(vehicleID,vehicleID, additionalCost, pickupSP->getActualTime(), newList);

     }
     else{
        delete pickupSP;
        delete dropoffSP;
    }

    return proposal;
}
