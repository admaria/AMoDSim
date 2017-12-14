#include <BaseCoord.h>
#include "Routing.h"


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
    TripRequest *propAccepted = new TripRequest(*tr);
    double pickupDeadline = tr->getPickupSP()->getTime() + tr->getPickupSP()->getMaxWaitingTime();
    double dropoffDeadline = tr->getDropoffSP()->getTime() + tr->getDropoffSP()->getMaxWaitingTime();
    double pickupActualTime = -1.0;
    double dropoffActualTime = -1.0;
    int vehicleID = -1;

    for(auto const &x : vehicleProposal)
    {
        StopPoint *sp = getRequestPickup(x.second, tr->getID());
        double actualTime = sp->getActualTime();

         if(actualTime <= pickupDeadline)
         {
             if(pickupActualTime == -1.0 ||  actualTime < pickupActualTime)
             {
                 vehicleID = x.first;
                 pickupActualTime = actualTime;
                 dropoffActualTime = x.second.back()->getActualTime();
             }
         }
    }

      if(pickupActualTime > -1)
      {
          EV << "Accepted request of vehicle "<< vehicleID << " for request: " << tr->getID() << " .Actual PICKUP time: " << pickupActualTime
             << "/Requested Pickup Deadline: " << pickupDeadline << " .Actual DROPOFF time: "
             << dropoffActualTime << "/Requested DropOFF Deadline: " << dropoffDeadline << endl;

          propAccepted->setVehicleID(vehicleID);
          propAccepted->getPickupSP()->setActualTime(pickupActualTime);
          propAccepted->getDropoffSP()->setActualTime(dropoffActualTime);

          if(rPerVehicle[vehicleID].empty())
              emit(newTripAssigned, propAccepted);

          if(pickupActualTime != simTime().dbl())
              rPerVehicle[vehicleID] = vehicleProposal[vehicleID];

          else
          {
              //The vehicle is already in the pickup node
              servedPickup[propAccepted->getID()] = propAccepted->getPickupSP();
              emit(waitingTime, 0.0);
              rPerVehicle[vehicleID].push_back(propAccepted->getDropoffSP());
          }

      }
      else
      {
          EV << "No vehicle in the system can serve the request " << tr->getID() << endl;
          uRequests[tr->getID()] = tr;
      }

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

    return NULL;
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
        EV <<"Distance from " << sp->getNodeID() << " to " << (*it)->getLocation() << " is > " <<  ((*it)->getTime() + (*it)->getMaxWaitingTime() - currentTime) << endl;
        if (getDistance(sp->getNodeID(), (*it)->getLocation()) > ((*it)->getTime() + (*it)->getMaxWaitingTime() - currentTime))
            beforeR.push_back((*it));
    }

    for (std::list<StopPoint*>::const_iterator it = lsp.begin(), end = lsp.end(); it != end; ++it) {
        EV <<"Distance from " << (*it)->getNodeID() << " to " << sp->getLocation() << " is > " <<  (sp->getTime() + sp->getMaxWaitingTime() - currentTime) << endl;
        if (getDistance((*it)->getNodeID(), sp->getLocation()) > (sp->getTime() + sp->getMaxWaitingTime() - currentTime))
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
                    EV << "The same node is in before and after list! Node is: " << (*it)->getNodeID() << endl;
                    isFeasible = false;
                    break;
                }

                if (getDistance((*it)->getNodeID(), (*it2)->getLocation()) > ((*it2)->getTime() + (*it2)->getMaxWaitingTime() - currentTime))
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
 * Get the time-distance from the sourceNode to the target Address.
 *
 * @param sourceNode The source node fullPath (e.g. AMoD.n[0])
 * @param targetAddress The destination address
 *
 * @return Time needed to reach the target address starting from the source node
 */
double BaseCoord::getDistance(std::string sourceNode, int targetAddress)
{
    cModule *sourceModule = getModuleByPath(sourceNode.c_str());
    if (sourceModule != NULL)
    {
        Routing *r = check_and_cast<Routing *>(sourceModule->getSubmodule("routing"));
        return r->getDistanceToTarget(targetAddress);
    }
    return -1;
}



/**
 * Get the space-distance from the sourceNode to the target Address.
 *
 * @param sourceNode The source node fullPath (e.g. AMoD.n[0])
 * @param targetAddress The destination address
 *
 * @return The space distance (in meters) from source node to the target one.
 */
double BaseCoord::getSpaceDistance(std::string sourceNode, int targetAddress)
{
    cModule *sourceModule = getModuleByPath(sourceNode.c_str());
    if (sourceModule != NULL)
    {
        Routing *r = check_and_cast<Routing *>(sourceModule->getSubmodule("routing"));
        return r->getSpaceDistanceToTarget(targetAddress);
    }
    return -1;
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
    StopPoint *sp = NULL;

    if ((rPerVehicle.find(vehicleID) != rPerVehicle.end()) && !(rPerVehicle[vehicleID].empty()))
    {
        rPerVehicle[vehicleID].pop_front();
        if (!rPerVehicle[vehicleID].empty())
        {
            StopPoint *r = rPerVehicle[vehicleID].front();
            sp = new StopPoint(*r);
        }
    }
    return sp;
}

/**
 * Get the current stop point for the specified vehicle.
 *
 * @param vehicleID
 * @return
 */
StopPoint* BaseCoord::getCurrentStopPoint(int vehicleID)
{
    StopPoint *sp = NULL;

    if ((rPerVehicle.find(vehicleID) != rPerVehicle.end()) && !(rPerVehicle[vehicleID].empty()))
    {
        StopPoint *r = rPerVehicle[vehicleID].front();
        sp = new StopPoint(*r);
        if(sp->getIsPickup())
        {
            servedPickup[r->getRequestID()] = r;
            emit(waitingTime, (simTime()-sp->getTime())/60);
        }
        else
        {
            double att = (simTime().dbl() - servedPickup[sp->getRequestID()]->getActualTime())/60; //ActualTripTime
            double ter = (getDistance(servedPickup[sp->getRequestID()]->getNodeID(), sp->getLocation()) / 60) / att; //Trip Efficiency Ratio
            double trip_distance = getSpaceDistance(servedPickup[sp->getRequestID()]->getNodeID(), sp->getLocation()) / 1000;
            emit(actualTripTime, att);
            emit(tripEfficiencyRatio, ter);
            emit(tripDistance, trip_distance);
        }
    }
    return sp;
}


/**
 * Emit statistical signal before end the simulation
 */
void BaseCoord::finish()
{
    char uRequestSignal[32];
    sprintf(uRequestSignal, "Unserved Requests up to %d", totrequests);
    recordScalar(uRequestSignal, uRequests.size());

    for(auto const& x : vehicles)
    {
        char distanceSignal[32];
        sprintf(distanceSignal, "vehicle%d-traveledDistance(km)", x.first->getID());
        recordScalar(distanceSignal, (x.first->getTraveledDistance())/1000);
    }

    //emit statistics related to trips not yet completed
    for(auto const& x : rPerVehicle)
    {
        for (std::list<StopPoint*>::const_iterator it = x.second.begin(), end = x.second.end(); it != end; ++it)
        {
            if((*it)->getIsPickup())
            {
                servedPickup[(*it)->getRequestID()] = (*it);
                emit(waitingTime, ((*it)->getActualTime() - (*it)->getTime())/60);
            }
            else
            {
                double att = ((*it)->getActualTime() - servedPickup[(*it)->getRequestID()]->getActualTime())/60; //ActualTripTime
                double ter = (getDistance(servedPickup[(*it)->getRequestID()]->getNodeID(), (*it)->getLocation()) / 60) / att; //Trip Efficiency Ratio
                double trip_distance = getSpaceDistance(servedPickup[(*it)->getRequestID()]->getNodeID(), (*it)->getLocation()) / 1000;
                emit(actualTripTime, att);
                emit(tripEfficiencyRatio, ter);
                emit(tripDistance, trip_distance);
            }
        }
    }
}

/**
 * Register the vehicle v in the node nodeID.
 *
 * @param v
 * @param nodeID The full path of the node (e.g. AMoD.n[0])
 */
void BaseCoord::registerVehicle(Vehicle *v, std::string nodeID)
{
    vehicles[v] = nodeID;
    EV << "Registered vehicle " << v->getID() << " in node: " << nodeID << endl;
}

/**
 * Get the last location where the vehicle was registered.
 *
 * @param vehicleID
 * @return the full path of the node (e.g. AMoD.n[0])
 */
std::string BaseCoord::getLastVehicleLocation(int vehicleID)
{
    for(auto const& x : vehicles)
    {
        if(x.first->getID() == vehicleID)
            return x.second;
    }
    return "";
}
