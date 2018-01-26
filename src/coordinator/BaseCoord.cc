#include <BaseCoord.h>

void BaseCoord::initialize()
{
    //Register signals
    tripRequest = registerSignal("tripRequest");
    newTripAssigned = registerSignal("newTripAssigned");
    waitingTime = registerSignal("waitingTime");
    actualTripTime = registerSignal("actualTripTime");
    tripEfficiencyRatio = registerSignal("tripEfficiencyRatio");
    tripDistance = registerSignal("tripDistance");

    totrequests = 0;
    alightingTime = getParentModule()->par("alightingTime").doubleValue();
    boardingTime = getParentModule()->par("boardingTime").doubleValue();
    netmanager = check_and_cast<AbstractNetworkManager *>(getParentModule()->getSubmodule("netmanager"));
    //netXsize = (getParentModule()->par("width").doubleValue() - 1) * (getParentModule()->par("nodeDistance").doubleValue());
    //netYsize = (getParentModule()->par("height").doubleValue() - 1) * (getParentModule()->par("nodeDistance").doubleValue());

    simulation.getSystemModule()->subscribe("tripRequest",this);

}

/**
 * Assign the new trip request to the vehicle which minimize the pickup waiting time.
 *
 * @param vehicleProposal The vehicles proposals
 * @param tr The new TripRequest
 *
 * @return The ID of the vehicle which will serve the request or -1 otherwise.
 */
int BaseCoord::minWaitingTimeAssignment (std::map<int,std::list<StopPoint*>> vehicleProposal, TripRequest *tr)
{
    double pickupDeadline = tr->getPickupSP()->getTime() + tr->getPickupSP()->getMaxDelay();
    double dropoffDeadline = tr->getDropoffSP()->getTime() + tr->getDropoffSP()->getMaxDelay();
    double pickupActualTime = -1.0;
    double dropoffActualTime = -1.0;
    int vehicleID = -1;
    StopPoint *sp = NULL;
    StopPoint *dsp = NULL;

    for(auto const &x : vehicleProposal)
    {
        sp = getRequestPickup(x.second, tr->getID());
        dsp =  getRequestDropOff(x.second, tr->getID());

        double actualPickupTime = sp->getActualTime();
        double actualDropoffTime = dsp->getActualTime();

         if(actualPickupTime <= pickupDeadline)
         {
             if(pickupActualTime == -1.0 ||  actualPickupTime < pickupActualTime)
             {
                 if(vehicleID != -1) //The current proposal is better than the previous one
                     cleanStopPointList(vehicleProposal[vehicleID]);

                 vehicleID = x.first;
                 pickupActualTime = actualPickupTime;
                 dropoffActualTime = actualDropoffTime;
             }
             else
                 cleanStopPointList(x.second); //Reject the current proposal (A better one has been accepted)
         }
         else
             cleanStopPointList(x.second); //Reject the current proposal: it does not respect the time constraints
    }

      if(pickupActualTime > -1)
      {
          EV << "Accepted request of vehicle "<< vehicleID << " for request: " << tr->getID() << " .Actual PICKUP time: " << pickupActualTime
             << "/Requested Pickup Deadline: " << pickupDeadline << " .Actual DROPOFF time: "
             << dropoffActualTime << "/Requested DropOFF Deadline: " << dropoffDeadline << endl;

          bool toEmit = false;
          if(rPerVehicle[vehicleID].empty())
          {
              //The node which handle the selected vehicle should be notified
              toEmit = true;
          }
          else
          {
              //clean the old stop point list assigned to the vehicle
              cleanStopPointList(rPerVehicle[vehicleID]);
          }

          if(pickupActualTime > simTime().dbl())
          {
              //The vehicle is not already in the pickup location
              rPerVehicle[vehicleID] = vehicleProposal[vehicleID];
          }

          else
          {
              //The vehicle is already in the pickup node
              servedPickup[tr->getID()] = new StopPoint(*tr->getPickupSP());
              emit(waitingTime, 0.0);
              vehicleProposal[vehicleID].remove(sp);

              rPerVehicle[vehicleID] = vehicleProposal[vehicleID];
          }
          if(toEmit)
              emit(newTripAssigned, (double)vehicleID);
      }
      else
      {
          EV << "No vehicle in the system can serve the request " << tr->getID() << endl;
          uRequests[tr->getID()] = new TripRequest(*tr);
          delete tr;
          return -1;
      }
      delete tr;

      return vehicleID;
}


/**
 * Get the pointer to the pickup SP related to the requestID.
 *
 * @param spList List of stop-point where look for the pickup.
 * @param requestID The ID of the TripRequest
 *
 * @return The StopPoint or NULL
 */
StopPoint* BaseCoord::getRequestPickup(std::list<StopPoint*> spList, int requestID)
{
    for (std::list<StopPoint*>::iterator it=spList.begin(); it != spList.end(); ++it)
    {
        if(((*it)->getRequestID() == requestID) && (*it)->getIsPickup())
            return (*it);
    }

    return nullptr;
}

/**
 * Get the pointer to the dropoff SP related to the requestID.
 *
 * @param spList List of stop-point where look for the dropoff.
 * @param requestID The ID of the TripRequest
 *
 * @return The StopPoint or NULL
 */
StopPoint* BaseCoord::getRequestDropOff(std::list<StopPoint*> spList, int requestID)
{
    for (std::list<StopPoint*>::iterator it=spList.begin(); it != spList.end(); ++it)
    {
        if(((*it)->getRequestID() == requestID) && !(*it)->getIsPickup())
            return (*it);
    }

    return nullptr;
}

/**
 * Evaluate if a stop point is "feasible" by a vehicle.
 *
 * @param vehicleID The vehicleID
 * @param sp The stop-point to evaluate
 *
 * @return true if feasible
 */
bool BaseCoord::eval_feasibility (int vehicleID, StopPoint* sp)
{
    //TODO return the beforeR and afterR

    std::list<StopPoint*> lsp = rPerVehicle[vehicleID];
    std::list<StopPoint*> afterR;
    std::list<StopPoint*> beforeR;
    double currentTime = simTime().dbl();
    bool isFeasible = true;

    for (std::list<StopPoint*>::const_iterator it = lsp.begin(), end = lsp.end(); it != end; ++it) {
        EV <<"Distance from " << sp->getLocation() << " to " << (*it)->getLocation() << " is > " <<  ((*it)->getTime() + (*it)->getMaxDelay() - currentTime) << endl;
        if (netmanager->getTimeDistance(sp->getLocation(), (*it)->getLocation()) > ((*it)->getTime() + (*it)->getMaxDelay() - currentTime))
            beforeR.push_back((*it));
    }

    for (std::list<StopPoint*>::const_iterator it = lsp.begin(), end = lsp.end(); it != end; ++it) {
        EV <<"Distance from " << (*it)->getLocation() << " to " << sp->getLocation() << " is > " <<  (sp->getTime() + sp->getMaxDelay() - currentTime) << endl;
        if (netmanager->getTimeDistance((*it)->getLocation(), sp->getLocation()) > (sp->getTime() + sp->getMaxDelay() - currentTime))
            afterR.push_back((*it));
    }

    if(beforeR.empty() || afterR.empty())
        isFeasible = true;

    else
    {
        for (std::list<StopPoint*>::const_iterator it = beforeR.begin(), end = beforeR.end(); it != end; ++it)
        {
            for(std::list<StopPoint*>::const_iterator it2 = afterR.begin(), end = afterR.end(); it2 != end; ++it2)
            {
                if((*it)->getLocation() == (*it2)->getLocation())
                {
                    EV << "The same node is in before and after list! Node is: " << (*it)->getLocation() << endl;
                    isFeasible = false;
                    break;
                }

                if (netmanager->getTimeDistance((*it)->getLocation(), (*it2)->getLocation()) > ((*it2)->getTime() + (*it2)->getMaxDelay() - currentTime))
                {
                    EV << "The request is not feasible for the vehicle " << vehicleID << endl;
                    isFeasible = false;
                    break;
                }
            }
            if(!isFeasible)
                break;
            EV << "The request could be feasible for the vehicle " << vehicleID << endl;
        }
    }

    return isFeasible;
}


/**
 * Get the next stop point for the specified vehicle.
 *
 * @param vehicleID
 *
 * @return
 */
StopPoint* BaseCoord::getNextStopPoint(int vehicleID)
{
    if ((rPerVehicle.find(vehicleID) != rPerVehicle.end()) && !(rPerVehicle[vehicleID].empty()))
    {
        rPerVehicle[vehicleID].pop_front();
        if (!rPerVehicle[vehicleID].empty())
        {
            StopPoint *r = rPerVehicle[vehicleID].front();
            return r;
        }
    }
    return NULL;
}

/**
 * Get the current stop point for the specified vehicle.
 *
 * @param vehicleID
 * @return
 */
StopPoint* BaseCoord::getCurrentStopPoint(int vehicleID)
{
    if ((rPerVehicle.find(vehicleID) != rPerVehicle.end()) && !(rPerVehicle[vehicleID].empty()))
    {
        StopPoint *r = rPerVehicle[vehicleID].front();

        if(r->getIsPickup())
        {
            servedPickup[r->getRequestID()] = r;
            emit(waitingTime, (simTime()-r->getTime())/60);
        }
        else
        {
            double att = (simTime().dbl() - servedPickup[r->getRequestID()]->getActualTime())/60; //ActualTripTime
            double ter = (netmanager->getTimeDistance(servedPickup[r->getRequestID()]->getLocation(), r->getLocation()) / 60) / att; //Trip Efficiency Ratio
            double trip_distance = netmanager->getSpaceDistance(servedPickup[r->getRequestID()]->getLocation(), r->getLocation()) / 1000;
            emit(actualTripTime, att);
            emit(tripEfficiencyRatio, ter);
            emit(tripDistance, trip_distance);
        }
        return r;
    }
    return NULL;
}


/**
 * Emit statistical signal before end the simulation
 */
void BaseCoord::finish()
{
    char uRequestSignal[32];
    sprintf(uRequestSignal, "Unserved Requests up to %d", totrequests);
    recordScalar(uRequestSignal, uRequests.size());

    for(std::map<Vehicle*, int>::iterator itr = vehicles.begin(); itr != vehicles.end(); itr++)
    {
        char distanceSignal[32];
        sprintf(distanceSignal, "vehicle%d-traveledDistance(km)", itr->first->getID());
        recordScalar(distanceSignal, (itr->first->getTraveledDistance())/1000);
        delete itr->first;
    }


    for(std::map<int, TripRequest*>::iterator itr = uRequests.begin(); itr != uRequests.end(); itr++)
            delete itr->second;

    for(std::map<int, std::list<StopPoint*>>::iterator itr = rPerVehicle.begin(); itr != rPerVehicle.end(); itr++)
            cleanStopPointList(itr->second);

    for(std::map<int, StopPoint*>::iterator itr = servedPickup.begin(); itr != servedPickup.end(); itr++)
             delete itr->second;

    //emit statistics related to trips not yet completed
//    for(auto const& x : rPerVehicle)
//    {
//        for (std::list<StopPoint*>::const_iterator it = x.second.begin(), end = x.second.end(); it != end; ++it)
//        {
//            EV << "SOME TRIP NOT COMPLETED!" << endl;
//            if((*it)->getIsPickup())
//            {
//                servedPickup[(*it)->getRequestID()] = (*it);
//                emit(waitingTime, ((*it)->getActualTime() - (*it)->getTime())/60);
//            }
//            else
//            {
//                double att = ((*it)->getActualTime() - servedPickup[(*it)->getRequestID()]->getActualTime())/60; //ActualTripTime
//                double ter = (getDistance(servedPickup[(*it)->getRequestID()]->getNodeID(), (*it)->getLocation()) / 60) / att; //Trip Efficiency Ratio
//                double trip_distance = getSpaceDistance(servedPickup[(*it)->getRequestID()]->getNodeID(), (*it)->getLocation()) / 1000;
//                emit(actualTripTime, att);
//                emit(tripEfficiencyRatio, ter);
//                emit(tripDistance, trip_distance);
//            }
//        }
//    }
}

/**
 * Register the vehicle v in a node.
 *
 * @param v
 * @param address The node address
 */
void BaseCoord::registerVehicle(Vehicle *v, int address)
{
    vehicles[v] = address;
    EV << "Registered vehicle " << v->getID() << " in node: " << address << endl;
}

/**
 * Get the last location where the vehicle was registered.
 *
 * @param vehicleID
 * @return the location address
 */
int BaseCoord::getLastVehicleLocation(int vehicleID)
{
    for(auto const& x : vehicles)
    {
        if(x.first->getID() == vehicleID)
            return x.second;
    }
    return -1;
}

/**
 * Get vehicle from its ID.
 *
 * @param vehicleID
 * @return pointer to the vehicle
 */
Vehicle* BaseCoord::getVehicleByID(int vehicleID)
{
    for(auto const& x : vehicles)
    {
        if(x.first->getID() == vehicleID)
            return x.first;
    }
    return nullptr;
}

/**
 * Delete unused dynamically allocated memory.
 *
 * @param spList The list of stop point
 */
void BaseCoord::cleanStopPointList(std::list<StopPoint*> spList)
{
    for(auto &it:spList) delete it;

    spList.clear();
}

/**
 * Check if a Trip Request is valid.
 *
 * @param tr The trip Request.
 * @return true if the request is valid.
 */
bool BaseCoord::isRequestValid(const TripRequest tr)
{
    bool valid = false;

    if(tr.getPickupSP() && tr.getDropoffSP() &&
            netmanager->isValidAddress(tr.getPickupSP()->getLocation()) && netmanager->isValidAddress(tr.getDropoffSP()->getLocation()))
                valid = true;
    return valid;

}

