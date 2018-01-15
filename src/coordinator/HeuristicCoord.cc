
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
        if(x.first->getSeats() >= tr->getPickupSP()->getNumberOfPassengers())
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
 * Returns the "optimal" stop point sorting.
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
    newTRdropoff->setNumberOfPassengers(-newTRpickup->getNumberOfPassengers());

    //The vehicle is empty
    if(old.empty())
    {
        EV << " The vehicle " << vehicleID << " is empty. Trying to add new request..." << endl;
        double timeToPickup = netmanager->getTimeDistance(getLastVehicleLocation(vehicleID), newTRpickup->getLocation()) + simTime().dbl();

        if(timeToPickup <= (newTRpickup->getTime() + newTRpickup->getMaxDelay()))
        {
            newTRpickup->setActualTime(timeToPickup);
            newTRdropoff->setActualTime(newTRpickup->getActualTime() + netmanager->getTimeDistance(newTRpickup->getLocation(), newTRdropoff->getLocation()) + boardingTime);
            newTRdropoff->setNumberOfPassengers(0);
            newList.push_back(newTRpickup);
            newList.push_back(newTRdropoff);
            EV << "New Pickup can be reached at " << newTRpickup->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRpickup->getTime() + newTRpickup->getMaxDelay()) << endl;
            EV << "New Dropoff can be reached at " << newTRdropoff->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRdropoff->getTime() + newTRdropoff->getMaxDelay()) << endl;
        }
        else
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
    }

    //The vehicle has 1 stop point
    else if(old.size() == 1)
    {
        EV << " The vehicle " << vehicleID << " has only 1 SP. Trying to push new request back..." << endl;

        StopPoint *last = old.back();
        double timeToPickup = netmanager->getTimeDistance(last->getLocation(), newTRpickup->getLocation()) + last->getActualTime() + alightingTime; //The last SP is a dropOff point.

        if(timeToPickup <= (newTRpickup->getTime() + newTRpickup->getMaxDelay()))
        {
            newTRpickup->setActualTime(timeToPickup);
            newTRdropoff->setActualTime(newTRpickup->getActualTime() + netmanager->getTimeDistance(newTRpickup->getLocation(), newTRdropoff->getLocation()) + boardingTime);
            newTRdropoff->setNumberOfPassengers(0);
            newList.push_back(last);
            newList.push_back(newTRpickup);
            newList.push_back(newTRdropoff);
            EV << "New Pickup can be reached at " << newTRpickup->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRpickup->getTime() + newTRpickup->getMaxDelay()) << endl;
            EV << "New Dropoff can be reached at " << newTRdropoff->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRdropoff->getTime() + newTRdropoff->getMaxDelay()) << endl;
        }
        else
        {
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
        }
    }

    //The vehicle has more stop points
    else
    {
        EV << " The vehicle " << vehicleID << " has more stop points..." << endl;

        //Get all the possible sorting within the Pickup
        std::list<StopPointOrderingProposal*> proposalsWithPickup = addStopPointToTrip(vehicleID, old, newTRpickup);

        std::list<StopPointOrderingProposal*> proposalsWithDropoff;
        StopPointOrderingProposal* toReturn = new StopPointOrderingProposal();

        if(!proposalsWithPickup.empty())
        {
            int cost = -1;
            for(std::list<StopPointOrderingProposal*>::const_iterator it2 = proposalsWithPickup.begin(), end2 = proposalsWithPickup.end(); it2 != end2; ++it2)
            {
                //Get all the possible sorting within the Dropoff
                proposalsWithDropoff = addStopPointToTrip(vehicleID, (*it2)->getSpList(), newTRdropoff);

                for(std::list<StopPointOrderingProposal*>::const_iterator it3 = proposalsWithDropoff.begin(), end3 = proposalsWithDropoff.end(); it3 != end3; ++it3)
                {
                    (*it3)->setAdditionalTime((*it3)->getAdditionalTime() + (*it2)->getAdditionalTime());
                    if(cost == -1 || (*it3)->getAdditionalTime() < cost)
                    {
                        cost = (*it3)->getAdditionalTime();
                        toReturn = (*it3);
                    }
                }
            }
            newList = toReturn->getSpList();
        }






//        std::list<StopPoint*> orderedListWithDropoff;
//
//        if(!orderedListWithPickup.empty())
//        {
//            std::list<StopPoint*> tmp;
//            for (std::list<StopPoint*>::const_iterator it = orderedListWithPickup.begin(), end = orderedListWithPickup.end(); it != end; ++it)
//            {
//                if((*it)->getRequestID() != newTR->getID());
//                else
//                {
//                    std::copy(it, end, std::back_insert_iterator<std::list<StopPoint*> >(tmp));
//                    break;
//                }
//            }
//            orderedListWithDropoff = minCostOrdering(vehicleID, tmp, newTRdropoff);
//            if(!orderedListWithDropoff.empty())
//            {
//                orderedListWithDropoff.pop_front();
//                for (std::list<StopPoint*>::const_iterator it = orderedListWithPickup.begin(), end = orderedListWithPickup.end(); it != end; ++it)
//                {
//                    newList.push_back(*it);
//                    if((*it)->getRequestID() == newTR->getID())
//                        break;
//                }
//                for (std::list<StopPoint*>::const_iterator it = orderedListWithDropoff.begin(), end = orderedListWithDropoff.end(); it != end; ++it)
//                    newList.push_back(*it);
//            }
//        }
    }
    return newList;
}




std::list<StopPointOrderingProposal*> HeuristicCoord::addStopPointToTrip(int vehicleID, std::list<StopPoint*> spl, StopPoint* newSP)
{
    int passengers = 0;
    int delay_BA = 0; // boarding/alighting delay.
    int vehicleSeats = getVehicleByID(vehicleID)->getSeats();
    double cost = -1.0;
    double actualTime = 0.0;
    double minResidual = 0.0;
    bool constrainedPosition = false;
    double distanceTo, distanceFrom, dt, df = 0.0;
    std::list<StopPointOrderingProposal*> mylist;

    std::list<double> residualTimes;
    std::list<StopPoint*>::const_iterator it = spl.begin();
    std::list<StopPoint*>::const_iterator it2, it3 = spl.end();

    if(newSP->getIsPickup())
    {
        delay_BA = boardingTime;
        residualTimes = getResidualTime(spl,-1);
    }
    else
    {
        delay_BA = alightingTime;
        residualTimes = getResidualTime(spl,newSP->getRequestID());
        //Move until the pickup stop point
        for (it; it != spl.end(); ++it) {
            if((*it)->getRequestID() == newSP->getRequestID())
                break;
        }
    }

    for (it; it!=spl.end(); ++it) {
        it2 = it;

        if((*it)->getNumberOfPassengers() + newSP->getNumberOfPassengers() <= vehicleSeats)
        {
            passengers = (*it)->getNumberOfPassengers();

            //Get the min residual-time from "it" to last SP
            if(residualTimes.size() > 1)
            {
                residualTimes.pop_front();
                minResidual = *(std::min_element(std::begin(residualTimes), std::end(residualTimes)));
                EV << " Min residual from " << (*it)->getLocation() << " is " <<  minResidual << endl;
            }
            //Arrived in last position
            else
            {
                residualTimes.clear();
                minResidual = newSP->getTime()+newSP->getMaxDelay() - (*it)->getActualTime();
                EV << " Min residual in last position is:  " << minResidual << endl;
                if(minResidual < 0)
                    break;
            }

            //Distance from prev SP to new SP
            if((*it)->getIsPickup())
                dt = netmanager->getTimeDistance((*it)->getLocation(), newSP->getLocation()) + boardingTime;
            else
                dt = netmanager->getTimeDistance((*it)->getLocation(), newSP->getLocation()) + alightingTime;

            EV << " Distance from " << (*it)->getLocation() << " to " <<  newSP->getLocation() << " is " << dt << endl;
            if((*it)->getActualTime() + dt > (newSP->getTime()+newSP->getMaxDelay()))
            {
                EV << "Stop search: from here will be unreachable within its deadline!" << endl;
                break;
            }

            //Distance from new SP to next SP
            if(it2 != (std::prev(spl.end())))
            {
                it2++;
                if((*it)->getLocation() == (*it2)->getLocation()) //Two stop point with the same location.
                {
                    EV << "Two Stop point with location " << (*it)->getLocation() << endl;
                    continue;
                }

                if(!newSP->getIsPickup() && (*it2)->getNumberOfPassengers() > vehicleSeats)
                    constrainedPosition = true;

                df = netmanager->getTimeDistance(newSP->getLocation(), (*it2)->getLocation()) + delay_BA;
                EV << " Distance from " << newSP->getLocation()  << " to " << (*it2)->getLocation() << " is " << df << endl;
                actualTime = (*it2)->getActualTime() - (*it)->getActualTime();
            }
            else
                df = actualTime = 0.0;

            double c = abs(df + dt - actualTime);
            EV << " The cost is " << c << ". The minResidual is: " << minResidual << endl;

            //The additional cost does not violate any deadline
            if(c < minResidual)
            {
                distanceTo = dt;
                distanceFrom = df;
                cost = c;
                it3 = it;
                std::list<StopPoint*> orderedList;
                StopPointOrderingProposal* proposal = new StopPointOrderingProposal();
                StopPoint* newSPcopy = new StopPoint(*newSP);

                /*Before new Stop point*/
                for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = std::next(it3); it != end; ++it) {
                    EV << " Before new SP pushing SP " << (*it)->getLocation() << ". Actual Time: " << (*it)->getActualTime() << endl;
                    orderedList.push_back(*it);
                }

                StopPoint *newListBack = orderedList.back();
                EV << " Adding new SP after " << newListBack->getLocation() << endl;
                if(newListBack->getIsPickup())
                    newSPcopy->setActualTime(newListBack->getActualTime()+distanceTo+boardingTime);
                else
                    newSPcopy->setActualTime(newListBack->getActualTime()+distanceTo+alightingTime);

                newSPcopy->setNumberOfPassengers(passengers + newSP->getNumberOfPassengers());
                EV << " New SP Max time: " << newSPcopy->getTime() + newSPcopy->getMaxDelay() << ". Actual Time: " << newSPcopy->getActualTime() << " PASSENGERS: " << newSPcopy->getNumberOfPassengers() << endl;
                orderedList.push_back(newSPcopy);

                //After new SP
                for (std::list<StopPoint*>::const_iterator it = std::next(it3), end = spl.end(); it != end; ++it) {
                    StopPoint* tmp = new StopPoint(**it);
                    double prevActualTime = tmp->getActualTime();
                    tmp->setActualTime(tmp->getActualTime() + cost);
                    tmp->setNumberOfPassengers(tmp->getNumberOfPassengers()+newSP->getNumberOfPassengers());
                    EV << " After new SP pushing " << tmp->getLocation() << ". Previous actualTime: " << prevActualTime << ". Current actualTime: " <<tmp->getActualTime() << " max time: " << tmp->getTime() + tmp->getMaxDelay() << endl;
                    orderedList.push_back(tmp);
                }

                proposal->setVehicleID(vehicleID);
                proposal->setAdditionalTime(cost);
                proposal->setSpList(orderedList);
                mylist.push_back(proposal);
            }
            else
                EV << "Additional Time not allowed!" << endl;

        }
        if(constrainedPosition)
            break;
    }
    return mylist;
}



//Get the additional time-cost allowed by each stop point
std::list<double> HeuristicCoord::getResidualTime(std::list<StopPoint*> spl, int requestID)
{
    std::list<double> residuals;

    //It is a pickup
    if(requestID == -1)
    {
        for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = spl.end(); it != end; ++it)
            residuals.push_back((*it)->getMaxDelay() + (*it)->getTime() - (*it)->getActualTime());
    }
    else
    {
        bool found = false;
        for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = spl.end(); it != end; ++it)
        {
            if(found)
                residuals.push_back((*it)->getMaxDelay() + (*it)->getTime() - (*it)->getActualTime());
            else if((*it)->getRequestID() == requestID)
            {
                residuals.push_back((*it)->getMaxDelay() + (*it)->getTime() - (*it)->getActualTime());
                found = true;
            }

        }
    }
    return residuals;
}





/**
 * Sort a list of stop point within a new stop point.
 * This sorting minimize the cost.
 *
 * @param spl The list of stop point without the new stop-point.
 * @param newSP The new stop-point which should be added in the list
 * @return The sorted list of stop-point
 */
//std::list<StopPoint*> HeuristicCoord::minCostOrdering(int vehicleID, std::list<StopPoint*> spl, StopPoint* newSP)
//{
//    int passengers = 0; //TODO Manage passengers.
//    int delay_BA = 0; // boarding/alighting delay.
//    int vehicleSeats = getVehicleByID(vehicleID).getSeats();
//    double cost = -1.0;
//    double actualTime = 0.0;
//    double minResidual = 0.0;
//    double distanceTo, distanceFrom, dt, df = 0.0;
//
//    std::list<double> residualTimes;
//    std::list<StopPoint*> orderedList;
//    std::list<StopPoint*>::const_iterator it2, it3 = spl.end();
//
//    if(newSP->getIsPickup())
//        delay_BA = boardingTime;
//    else
//        delay_BA = alightingTime;
//
//    //Get the additional time-cost allowed by each stop point
//    for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = spl.end(); it != end; ++it)
//        residualTimes.push_back((*it)->getMaxDelay() + (*it)->getTime() - (*it)->getActualTime());
//
//    for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = spl.end(); it != end; ++it) {
//        it2 = it;
//
//        if((*it)->getNumberOfPassengers() + newSP->getNumberOfPassengers() > vehicleSeats)
//        {
//            EV << " The vehicle does not have enough seats " << (*it)->getNodeID() << " is " <<  minResidual << endl;
//        }
//        //Get the min residual-time from "it" to last SP
//        if(residualTimes.size() > 1)
//        {
//            residualTimes.pop_front();
//            minResidual = *(std::min_element(std::begin(residualTimes), std::end(residualTimes)));
//            EV << " Min residual from " << (*it)->getNodeID() << " is " <<  minResidual << endl;
//        }
//        //Arrived in last position
//        else
//        {
//            residualTimes.clear();
//            minResidual = newSP->getTime()+newSP->getMaxDelay() - (*it)->getActualTime();
//            EV << " Min residual in last position is:  " << minResidual << endl;
//        }
//
//        //Distance from prev SP to new SP
//        dt = getDistance((*it)->getNodeID(), newSP->getLocation());//TODO Add boarding/alig time
//        EV << " Distance from " << (*it)->getNodeID() << " to " <<  newSP->getLocation() << " is " << dt << endl;
//        if((*it)->getActualTime() + dt > (newSP->getTime()+newSP->getMaxDelay()))
//        {
//            EV << "Stop search: from here will be unreachable within its deadline!" << endl;
//            break;
//        }
//
//        //Distance from new SP to next SP
//        if(it2 != (std::prev(end)))
//        {
//            it2++;
//            df = getDistance(newSP->getNodeID(), (*it2)->getLocation()) + delay_BA;
//            EV << " Distance from " << newSP->getLocation()  << " to " << (*it2)->getLocation() << " is " << df << endl;
//            actualTime = (*it2)->getActualTime() - (*it)->getActualTime();
//        }
//        else
//            df = actualTime = 0.0;
//
//        double c = abs(df + dt - actualTime);
//        EV << " The cost is " << c << ". The minResidual is: " << minResidual << endl;
//
//        if(c < minResidual)
//        {
//            EV << " The cost is minor than residual. Is minor than previous cost?" << cost << endl;
//            if (cost == -1 || c < cost)
//            {
//                distanceTo = dt;
//                distanceFrom = df;
//                cost = c;
//                it3 = it;
//            }
//        }
//        else
//            EV << "Additional Time not allowed!" << endl;
//
//    }
//
//    if(it3 == spl.end())
//        EV << "The vehicle can not serve the new SP " << newSP->getLocation() << endl;
//
//    else{
//        /*Before new Stop point*/
//        for (std::list<StopPoint*>::const_iterator it = spl.begin(), end = std::next(it3); it != end; ++it) {
//            EV << " Before new SP pushing SP " << (*it)->getLocation() << ". Actual Time: " << (*it)->getActualTime() << endl;
//            orderedList.push_back(*it);
//        }
//
//        StopPoint *newListBack = orderedList.back();
//        EV << " Adding new SP after " << newListBack->getLocation() << endl;
//        if(newListBack->getIsPickup())
//            newSP->setActualTime(newListBack->getActualTime()+distanceTo+boardingTime);
//        else
//            newSP->setActualTime(newListBack->getActualTime()+distanceTo+alightingTime);
//
//        EV << " New SP Max time: " << newSP->getTime() + newSP->getMaxDelay() << ". Actual Time: " << newSP->getActualTime() << endl;
//        orderedList.push_back(newSP);
//
//        //After new SP
//        for (std::list<StopPoint*>::const_iterator it = std::next(it3), end = spl.end(); it != end; ++it) {
//            StopPoint* tmp = new StopPoint(**it);
//            double prevActualTime = tmp->getActualTime();
//            tmp->setActualTime(tmp->getActualTime() + cost);
//            EV << " After new SP pushing " << tmp->getLocation() << ". Previous actualTime: " << prevActualTime << ". Current actualTime: " <<tmp->getActualTime() << " max time: " << tmp->getTime() + tmp->getMaxDelay() << endl;
//            orderedList.push_back(tmp);
//        }
//    }
//    return orderedList;
//}










