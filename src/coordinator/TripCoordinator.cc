#include "TripCoordinator.h"
#include "TripRequest.h"
#include "Routing.h"

int netXsize;
int netYsize;

void TripCoordinator::initialize()
{
    tripRequest = registerSignal("tripRequest");
    newTripAssigned = registerSignal("newTripAssigned");
    waitingTime = registerSignal("waitingTime");
    netXsize = (getParentModule()->par("width").doubleValue() - 1) * (getParentModule()->par("nodeDistance").doubleValue());
    netYsize = (getParentModule()->par("height").doubleValue() - 1) * (getParentModule()->par("nodeDistance").doubleValue());
    totrequests = 0;

    simulation.getSystemModule()->subscribe("tripRequest",this);

}

Define_Module(TripCoordinator);


void TripCoordinator::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj){

  if(signalID == tripRequest)
  {
      TripRequest *pk = check_and_cast<TripRequest *>(obj);
      EV << "New TRIP request from: " << source->getFullPath() << endl;
      totrequests++;

      std::map<int, double> tmp = getMinDistanceToPickup(pk->getPickupSP());
      double maxWaitingTime = pk->getPickupSP()->getMaxWaitingTime();
      TripRequest *propAccepted = NULL;
      double distToPickup = -1;

      for(auto const &x : tmp)
      {
         double dist = x.second;
         if(dist <= maxWaitingTime)
         {
             if(distToPickup == -1 ||  dist < distToPickup)
             {
                 propAccepted = new TripRequest(*pk);
                 propAccepted->setVehicleID(x.first);
                 propAccepted->getPickupSP()->setActualTime(simTime().dbl() + dist);
                 propAccepted->getDropoffSP()->setActualTime(propAccepted->getPickupSP()->getActualTime() + (pk->getDropoffSP()->getTime() - pk->getPickupSP()->getTime()));
                 propAccepted->getDropoffSP()->setPassenger(-propAccepted->getPickupSP()->getPassenger());
                 distToPickup = dist;
             }
         }
      }

      if(distToPickup > -1)
      {
          EV << "Accepted request of vehicle "<< propAccepted->getVehicleID() << " for request: " << pk->getID() << ". Actual PICKUP time: " << propAccepted->getPickupSP()->getActualTime()
                  << "/Requested Pickup Deadline: " << pk->getPickupSP()->getTime() + pk->getPickupSP()->getMaxWaitingTime() << ". Actual DROPOFF time: "
                  << propAccepted->getDropoffSP()->getActualTime() << "/Requested DropOFF Deadline: " << pk->getDropoffSP()->getTime() + pk->getDropoffSP()->getMaxWaitingTime() << endl;

          if(rPerVehicle[propAccepted->getVehicleID()].empty())
              emit(newTripAssigned, propAccepted);

          if(distToPickup != 0)
              rPerVehicle[propAccepted->getVehicleID()].push_back(propAccepted->getPickupSP());

          else
          {
              //The vehicle is already in the pickup node
              emit(waitingTime, 0);
          }

          rPerVehicle[propAccepted->getVehicleID()].push_back(propAccepted->getDropoffSP());
      }
      else
      {
          EV << "No vehicle in the system can serve the request " << pk->getID() << endl;
          uRequests[pk->getID()] = pk;
      }
  }
}

/**
 * Get for each vehicle the min time needed to reach the pickupSP
 */
std::map<int,double> TripCoordinator::getMinDistanceToPickup(StopPoint *pickupSP)
{
    std::map<int,double> distPerVechicle;

    for (auto const &x : vehicles)
    {
        //Check if the vehicle has enough seats to serve the request
        if(x.first->getSeats() >= pickupSP->getPassenger())
        {
            //The vehicle has not other stop point
            if(rPerVehicle.find(x.first->getID()) == rPerVehicle.end() || rPerVehicle[x.first->getID()].empty())
            {
                EV << "The vehicle " << x.first->getID() << " has not other stop points!" << endl;
                double dst_to_pickup = getDistance(x.second, pickupSP->getLocation());
                if (dst_to_pickup != -1)
                {
                   distPerVechicle[x.first->getID()] = dst_to_pickup;
                   EV << "Time needed to vehicle: " << x.first->getID() << " to reach pickup: " << pickupSP->getLocation() << " is: " << distPerVechicle[x.first->getID()]/60 << " minutes." << endl;
                }
            }

            else
            {
                EV << "The vehicle " << x.first->getID() << " has other stop points!" << endl;

                //Get last stop point for the vehicle
                StopPoint *sp = rPerVehicle[x.first->getID()].back();
                Routing *r = check_and_cast<Routing *>(getModuleByPath(sp->getNodeID().c_str())->getSubmodule("routing"));
                double dst = r->getDistanceToTarget(pickupSP->getLocation()) + (sp->getActualTime() - simTime().dbl());
                int dst_minutes = dst/60;
                EV << "LAST stop point: " << sp->getLocation() << "; LAST SP Actual time: " << sp->getActualTime() << "; Distance to new SP: " << dst_minutes << " minutes" << endl;
                distPerVechicle[x.first->getID()] = dst;
            }
        }
    }
    return distPerVechicle;
}


/**
 * Get the time-distance from the sourceNode to the targetNode
 */
double TripCoordinator::getDistance(std::string sourceNode, int targetAddress)
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
 * Give the next stop point for the specific vehicle
 */
StopPoint* TripCoordinator::getNextStopPoint(int vehicleID)
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
 * Give the current stop point for the specific vehicle
 */
StopPoint* TripCoordinator::getCurrentStopPoint(int vehicleID)
{
    StopPoint *sp = NULL;

    if ((rPerVehicle.find(vehicleID) != rPerVehicle.end()) && !(rPerVehicle[vehicleID].empty()))
    {
        StopPoint *r = rPerVehicle[vehicleID].front();
        sp = new StopPoint(*r);
        if(sp->getIsPickup())
            emit(waitingTime, (simTime()-sp->getTime())/60);
    }
    return sp;
}


/**
 * Emit statistical signal before end the simulation
 */
void TripCoordinator::finish()
{
    char uRequestSignal[32];
    sprintf(uRequestSignal, "Unserved Requests up to %d", totrequests);
    recordScalar(uRequestSignal, uRequests.size());

    for(auto const& x : vehicles)
    {
        char distanceSignal[32];
        sprintf(distanceSignal, "vehicle%d-traveledDistance", x.first->getID());
        recordScalar(distanceSignal, x.first->getTraveledDistance());
    }
}

/**
 * Register the vehicle v in the node nodeID
 */
void TripCoordinator::registerVehicle(Vehicle *v, std::string nodeID)
{
    vehicles[v] = nodeID;
    EV << "Registered vehicle " << v->getID() << " in node: " << nodeID << endl;
}

