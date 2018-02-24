/*
############################################
##              __  __       _____        ##
##        /\   |  \/  |     |  __ \       ##
##       /  \  | \  / | ___ | |  | |      ##
##      / /\ \ | |\/| |/ _ \| |  | |      ##
##     / ____ \| |  | | (_) | |__| |      ##
##    /_/    \_\_|  |_|\___/|_____/       ##
##                                        ##
## Author:                                ##
##    Andrea Di Maria                     ##
##    <andrea.dimaria90@gmail.com>        ##
############################################
*/

#include <HeuristicCoord.h>
#include <algorithm>

Define_Module(HeuristicCoord);

double curTimeCost = -1.0;

void HeuristicCoord::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    if(signalID == tripRequest)
    {
      TripRequest *tr = check_and_cast<TripRequest *>(obj);
      EV << "New TRIP request from: " << source->getFullPath() << endl;

      if(isRequestValid(*tr))
      {
          pendingRequests.insert(std::make_pair(tr->getID(), new TripRequest(*tr)));
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

void HeuristicCoord::handleTripRequest(TripRequest *tr)
{
    std::map<int,std::list<StopPoint*>> vehicleProposals;

    for (auto const &x : vehicles)
    {
        //Check if the vehicle has enough seats to serve the request
        if(x.first->getSeats() >= tr->getPickupSP()->getNumberOfPassengers())
        {
            std::list<StopPoint *> tmp = eval_requestAssignment(x.first->getID(), tr);
            if(!tmp.empty())
            {
                if(!vehicleProposals.empty())
                {
                    cleanStopPointList(vehicleProposals.begin()->second);
                    vehicleProposals.clear();
                }
                vehicleProposals[x.first->getID()] = tmp;
            }
        }
    }

    curTimeCost = -1.0;
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
    StopPoint* newTRpickup = new StopPoint(*tr->getPickupSP());
    StopPoint* newTRdropoff = new StopPoint(*tr->getDropoffSP());
    newTRdropoff->setNumberOfPassengers(-newTRpickup->getNumberOfPassengers());

    //The vehicle is empty
    if(rPerVehicle.find(vehicleID) == rPerVehicle.end() || old.empty())
    {
        EV << "The vehicle " << vehicleID << " has not other stop points!" << endl;
        double timeToPickup = netmanager->getTimeDistance(getLastVehicleLocation(vehicleID), newTRpickup->getLocation()) + simTime().dbl();

        if(timeToPickup <= (newTRpickup->getTime() + newTRpickup->getMaxDelay()) &&
                (curTimeCost == -1 || timeToPickup < curTimeCost))
        {
            newTRpickup->setActualTime(timeToPickup);
            newTRpickup->setActualNumberOfPassengers(newTRpickup->getNumberOfPassengers());
            newTRdropoff->setActualTime(newTRpickup->getActualTime() + netmanager->getTimeDistance(newTRpickup->getLocation(), newTRdropoff->getLocation()) + boardingTime);
            newTRdropoff->setActualNumberOfPassengers(0);
            newList.push_back(newTRpickup);
            newList.push_back(newTRdropoff);
            curTimeCost = timeToPickup;
            EV << "New Pickup can be reached at " << newTRpickup->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRpickup->getTime() + newTRpickup->getMaxDelay()) << endl;
            EV << "New Dropoff can be reached at " << newTRdropoff->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRdropoff->getTime() + newTRdropoff->getMaxDelay()) << endl;
        }
        else
        {
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
            delete newTRpickup;
            delete newTRdropoff;
        }
    }

    //The vehicle has 1 stop point
    else if(old.size() == 1)
    {
        EV << " The vehicle " << vehicleID << " has only 1 SP. Trying to push new request back..." << endl;

        StopPoint *last = new StopPoint(*old.back());
        double timeToPickup = netmanager->getTimeDistance(last->getLocation(), newTRpickup->getLocation()) + last->getActualTime() + alightingTime; //The last SP is a dropOff point.

        if(timeToPickup <= (newTRpickup->getTime() + newTRpickup->getMaxDelay()) &&
                (curTimeCost == -1 || timeToPickup < curTimeCost))
        {
            newTRpickup->setActualTime(timeToPickup);
            newTRdropoff->setActualTime(newTRpickup->getActualTime() + netmanager->getTimeDistance(newTRpickup->getLocation(), newTRdropoff->getLocation()) + boardingTime);
            newTRpickup->setActualNumberOfPassengers(newTRpickup->getNumberOfPassengers());
            newTRdropoff->setActualNumberOfPassengers(0);
            newList.push_back(last);
            newList.push_back(newTRpickup);
            newList.push_back(newTRdropoff);
            curTimeCost = timeToPickup;
            EV << "New Pickup can be reached at " << newTRpickup->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRpickup->getTime() + newTRpickup->getMaxDelay()) << endl;
            EV << "New Dropoff can be reached at " << newTRdropoff->getActualTime() << " by the vehicle " << vehicleID << ". Max allowed time is: " << (newTRdropoff->getTime() + newTRdropoff->getMaxDelay()) << endl;
        }
        else
        {
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
            delete newTRpickup;
            delete newTRdropoff;
            delete last;
        }
    }

    //The vehicle has more stop points
    else
    {
        EV << " The vehicle " << vehicleID << " has more stop points..." << endl;

        //Get all the possible sorting within the Pickup
        std::list<StopPointOrderingProposal*> proposalsWithPickup = addStopPointToTrip(vehicleID, old, newTRpickup);
        delete newTRpickup;

        std::list<StopPointOrderingProposal*> proposalsWithDropoff;
        StopPointOrderingProposal* toReturn = new StopPointOrderingProposal();

        if(!proposalsWithPickup.empty())
        {
            int cost = -1;
            EV << "STARTING DROPOFF EVALUATION..." << endl;
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
                        curTimeCost = (*it2)->getAdditionalTime();
                        delete toReturn;

                        toReturn = (*it3);
                    }
                    else
                        delete *it3;
                }
                delete *it2;
            }
            delete newTRdropoff;
            newList = toReturn->getSpList();
        }

        else
        {
            EV << " The vehicle " << vehicleID << " can not serve the Request " << tr->getID() << endl;
            delete newTRdropoff;
            delete toReturn;
        }
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

        if((*it)->getActualNumberOfPassengers() + newSP->getNumberOfPassengers() <= vehicleSeats)
        {
            passengers = (*it)->getActualNumberOfPassengers();

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
                if((*it)->getLocation() == (*it2)->getLocation() && (!(*it2)->getIsPickup())) //Two stop point with the same location.
                {
                    EV << "Two Stop point with location " << (*it)->getLocation() << endl;
                    continue;
                }

                if(!newSP->getIsPickup() && (*it2)->getActualNumberOfPassengers() > vehicleSeats)
                {
                    EV << "The new DropOFF MUST be put before location " << (*it2)->getLocation() << " because it has " << (*it2)->getActualNumberOfPassengers() << " passengers!" << endl;
                    constrainedPosition = true;
                }

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
                if((*it)->getIsPickup() || (cost == -1 || c<cost))
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
                        EV << " Before new SP pushing SP " << (*it)->getLocation() << ". Actual Time: " << (*it)->getActualTime() << ". PASSENGERS: " << (*it)->getActualNumberOfPassengers() << endl;
                        orderedList.push_back(new StopPoint(**it));
                    }

                    StopPoint *newListBack = orderedList.back();
                    EV << " Adding new SP after " << newListBack->getLocation() << endl;
                    if(newListBack->getIsPickup())
                        newSPcopy->setActualTime(newListBack->getActualTime()+distanceTo+boardingTime);
                    else
                        newSPcopy->setActualTime(newListBack->getActualTime()+distanceTo+alightingTime);

                    newSPcopy->setActualNumberOfPassengers(passengers + newSP->getNumberOfPassengers());
                    EV << " New SP Max time: " << newSPcopy->getTime() + newSPcopy->getMaxDelay() << ". Actual Time: " << newSPcopy->getActualTime() << " PASSENGERS: " << newSPcopy->getActualNumberOfPassengers() << endl;
                    orderedList.push_back(newSPcopy);

                    //After new SP
                    for (std::list<StopPoint*>::const_iterator it = std::next(it3), end = spl.end(); it != end; ++it) {
                        StopPoint* tmp = new StopPoint(**it);
                        double prevActualTime = tmp->getActualTime();
                        tmp->setActualTime(tmp->getActualTime() + cost);
                        tmp->setActualNumberOfPassengers(tmp->getActualNumberOfPassengers()+newSP->getNumberOfPassengers());
                        EV << " After new SP pushing " << tmp->getLocation() << ". Previous actualTime: " << prevActualTime << ". Current actualTime: " <<tmp->getActualTime() << " max time: " << tmp->getTime() + tmp->getMaxDelay() << ". PASSENGERS: " << tmp->getActualNumberOfPassengers()<< endl;
                        orderedList.push_back(tmp);
                    }

                    proposal->setVehicleID(vehicleID);
                    proposal->setAdditionalTime(cost);
                    proposal->setSpList(orderedList);
                    if(!(*it)->getIsPickup() && !mylist.empty())
                    {
                        StopPointOrderingProposal* toDelete = mylist.front();
                        mylist.pop_front();
                        delete toDelete;
                    }
                    mylist.push_back(proposal);
                }
            }
            else
                EV << "Additional Time not allowed!" << endl;

        }
        else
            EV << "Too many passengers after SP in location " << (*it)->getLocation() << " and ReqiestID " << (*it)->getRequestID() << endl;

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





