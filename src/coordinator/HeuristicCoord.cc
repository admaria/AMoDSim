
#include <HeuristicCoord.h>
#include <algorithm>

Define_Module(HeuristicCoord);

void HeuristicCoord::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    if(signalID == tripRequest)
    {
      TripRequest *tr = check_and_cast<TripRequest *>(obj);
      EV << "New TRIP request from: " << source->getFullPath() << endl;
      totrequests++;

      handleTripRequest(tr);
    }
}

void HeuristicCoord::handleTripRequest(TripRequest *tr)
{
    std::map<int,std::list<StopPoint*>> vehicleProposals;

    for (auto const &x : vehicles)
    {
        //Check if the vehicle has enough seats to serve the request
        if(x.first->getSeats() >= tr->getPickupSP()->getPassenger())
        {
            std::list<StopPoint *> tmp = eval_requestAssignment(x.first->getID(), tr);
            EV << " Assignment proposal by vehicle " << x.first->getID() << " has size: " << tmp.size() << endl;
            if(!tmp.empty())
                vehicleProposals[x.first->getID()] = tmp;
        }
    }

    //Assign the request to the vehicle which minimize the waiting time
    minWaitingTimeAssignment(vehicleProposals, tr);

}


/**
 * Sort the stop-points related to the specified vehicle
 * including the new request's pickup and dropoff point, if feasible.
 *
 * @param vehicleID The vehicleID
 * @param tr The new trip request
 *
 * @return The list of stop-point related to the vehicle.
 */
std::list<StopPoint*> HeuristicCoord::eval_requestAssignment(int vehicleID, TripRequest* tr)
{
    std::list<StopPoint*> old = rPerVehicle[vehicleID];
    std::list<StopPoint*> newList;
    TripRequest *newTR = new TripRequest(*tr);
    StopPoint* newTRpickup = newTR->getPickupSP();
    StopPoint* newTRdropoff = newTR->getDropoffSP();

    if(old.empty())
    {
        EV << " The vehicle " << vehicleID << " is empty. Trying to add new request..." << endl;
        double timeToPickup = getDistance(getLastVehicleLocation(vehicleID), newTRpickup->getLocation()) + simTime().dbl();

        if(timeToPickup <= (newTRpickup->getTime() + newTRpickup->getMaxDelay()))
        {
            newTRpickup->setActualTime(timeToPickup);
            newTRdropoff->setActualTime(newTRpickup->getActualTime() + getDistance(newTRpickup->getNodeID(), newTRdropoff->getLocation()) + boardingTime);
            newList.push_back(newTRpickup);
            newList.push_back(newTRdropoff);
            EV << "NEW LIST SIZE: " << newList.size() << endl;
            EV << "New Pickup can be reached at " << newTRpickup->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRpickup->getTime() + newTRpickup->getMaxDelay()) << endl;
            EV << "New Dropoff can be reached at " << newTRdropoff->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRdropoff->getTime() + newTRdropoff->getMaxDelay()) << endl;
        }
        else
        {
            EV << "NEW LIST SIZE: " << newList.size() << endl;
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
        }
    }

    else if(old.size() == 1)
    {
        EV << " The vehicle " << vehicleID << " has only 1 SP. Trying to push new request back..." << endl;

        StopPoint *last = old.back();
        double timeToPickup = getDistance(last->getNodeID(), newTRpickup->getLocation()) + last->getActualTime() + alightingTime; //The last SP is a dropOff point.

        if(timeToPickup <= (newTRpickup->getTime() + newTRpickup->getMaxDelay()))
        {
            newTRpickup->setActualTime(timeToPickup);
            newTRdropoff->setActualTime(newTRpickup->getActualTime() + getDistance(newTRpickup->getNodeID(), newTRdropoff->getLocation()) + boardingTime);
            newList.push_back(last);
            newList.push_back(newTRpickup);
            newList.push_back(newTRdropoff);
            EV << "NEW LIST SIZE: " << newList.size() << endl;
            EV << "New Pickup can be reached at " << newTRpickup->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRpickup->getTime() + newTRpickup->getMaxDelay()) << endl;
            EV << "New Dropoff can be reached at " << newTRdropoff->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRdropoff->getTime() + newTRdropoff->getMaxDelay()) << endl;
        }
        else
        {
            EV << "NEW LIST SIZE: " << newList.size() << endl;
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
        }
    }

    else
    {
        EV << " The vehicle " << vehicleID << " has more stop points..." << endl;
        std::list<StopPoint*> orderedListWithPickup = minCostOrdering(old, newTRpickup);
        std::list<StopPoint*> orderedListWithDropoff;

        if(!orderedListWithPickup.empty())
        {
            std::list<StopPoint*> tmp;
            for (std::list<StopPoint*>::const_iterator it = orderedListWithPickup.begin(), end = orderedListWithPickup.end(); it != end; ++it)
            {
                if((*it)->getRequestID() != newTR->getID());
                else
                {
                    std::copy(it, end, std::back_insert_iterator<std::list<StopPoint*> >(tmp));
                    break;
                }
            }
            orderedListWithDropoff = minCostOrdering(tmp, newTRdropoff);
            if(!orderedListWithDropoff.empty())
            {
                orderedListWithDropoff.pop_front();
                for (std::list<StopPoint*>::const_iterator it = orderedListWithPickup.begin(), end = orderedListWithPickup.end(); it != end; ++it)
                {
                    newList.push_back(*it);
                    if((*it)->getRequestID() == newTR->getID())
                        break;
                }
                for (std::list<StopPoint*>::const_iterator it = orderedListWithDropoff.begin(), end = orderedListWithDropoff.end(); it != end; ++it)
                    newList.push_back(*it);
            }
        }
    }
    return newList;
}


/**
 * Sort a list of stop point within a new stop point.
 * This sorting minimize the cost.
 *
 * @param spl The list of stop point without the new stop-point.
 * @param newSP The new stop-point which should be added in the list
 * @return The sorted list of stop-point
 */
std::list<StopPoint*> HeuristicCoord::minCostOrdering(std::list<StopPoint*> spl, StopPoint* newSP)
{
    int passengers = 0; //TODO Manage passengers.
    int delay_BA = 0; // boarding/alighting delay.
    double cost = -1.0;
    double actualTime = 0.0;
    double minResidual = 0.0;
    double distanceTo, distanceFrom, dt, df = 0.0;

    std::list<double> residualTimes;
    std::list<StopPoint*> orderedList;
    std::list<StopPoint*>::const_iterator it2, it3 = spl.end();

    if(newSP->getIsPickup())
        delay_BA = boardingTime;
    else
        delay_BA = alightingTime;

    //Get the additional time-cost allowed by each stop point
    for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = spl.end(); it != end; ++it)
        residualTimes.push_back((*it)->getMaxDelay() + (*it)->getTime() - (*it)->getActualTime());

    for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = spl.end(); it != end; ++it) {
        it2 = it;

        //Get the min residual-time from "it" to last SP
        if(residualTimes.size() > 1)
        {
            residualTimes.pop_front();
            minResidual = *(std::min_element(std::begin(residualTimes), std::end(residualTimes)));
            EV << " Min residual from " << (*it)->getNodeID() << " is " <<  minResidual << endl;
        }
        //Arrived in last position
        else
        {
            residualTimes.clear();
            minResidual = newSP->getTime()+newSP->getMaxDelay() - (*it)->getActualTime();
            EV << " Min residual in last position is:  " << minResidual << endl;
        }

        //Distance from prev SP to new SP
        dt = getDistance((*it)->getNodeID(), newSP->getLocation());//TODO Add boarding/alig time
        EV << " Distance from " << (*it)->getNodeID() << " to " <<  newSP->getLocation() << " is " << dt << endl;
        if((*it)->getActualTime() + dt > (newSP->getTime()+newSP->getMaxDelay()))
        {
            EV << "Stop search: from here will be unreachable within its deadline!" << endl;
            break;
        }

        //Distance from new SP to next SP
        if(it2 != (std::prev(end)))
        {
            it2++;
            df = getDistance(newSP->getNodeID(), (*it2)->getLocation()) + delay_BA;
            EV << " Distance from " << newSP->getLocation()  << " to " << (*it2)->getLocation() << " is " << df << endl;
            actualTime = (*it2)->getActualTime() - (*it)->getActualTime();
        }
        else
            df = actualTime = 0.0;

        double c = abs(df + dt - actualTime);
        EV << " The cost is " << c << ". The minResidual is: " << minResidual << endl;

        if(c < minResidual)
        {
            EV << " The cost is minor than residual. Is minor than previous cost?" << cost << endl;
            if (cost == -1 || c < cost)
            {
                distanceTo = dt;
                distanceFrom = df;
                cost = c;
                it3 = it;
            }
        }
        else
            EV << "Additional Time not allowed!" << endl;

    }

    if(it3 == spl.end())
        EV << "The vehicle can not serve the new SP " << newSP->getLocation() << endl;

    else{
        /*Before new Stop point*/
        for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = std::next(it3); it != end; ++it) {
            EV << " Before new SP pushing SP " << (*it)->getLocation() << ". Actual Time: " << (*it)->getActualTime() << endl;
            orderedList.push_back(*it);
        }

        StopPoint *newListBack = orderedList.back();
        EV << " Adding new SP after " << newListBack->getLocation() << endl;
        if(newListBack->getIsPickup())
            newSP->setActualTime(newListBack->getActualTime()+distanceTo+boardingTime);
        else
            newSP->setActualTime(newListBack->getActualTime()+distanceTo+alightingTime);

        EV << " New SP Max time: " << newSP->getTime() + newSP->getMaxDelay() << ". Actual Time: " << newSP->getActualTime() << endl;
        orderedList.push_back(newSP);

        //After new SP
        for (std::list<StopPoint*>::const_iterator it = std::next(it3), end = spl.end(); it != end; ++it) {
            StopPoint* tmp = new StopPoint(**it);
            double prevActualTime = tmp->getActualTime();
            tmp->setActualTime(tmp->getActualTime() + cost);
            EV << " After new SP pushing " << tmp->getLocation() << ". Previous actualTime: " << prevActualTime << ". Current actualTime: " <<tmp->getActualTime() << " max time: " << tmp->getTime() + tmp->getMaxDelay() << endl;
            orderedList.push_back(tmp);
        }
    }
    return orderedList;
}
