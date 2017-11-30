
#ifndef __ROUTING_TRIPCOORDINATOR_H_
#define __ROUTING_TRIPCOORDINATOR_H_

#include <omnetpp.h>
#include "TripRequest.h"
#include "Vehicle.h"
#include <list>

class TripCoordinator : public cSimpleModule, cListener
{
private:
    int totrequests;

    simsignal_t tripRequest;
    simsignal_t newTripAssigned;
    simsignal_t waitingTime;

    std::map<Vehicle*, std::string> vehicles; //Vehicle -> nodeID

    typedef std::map<int,std::list<StopPoint*>> RequestsPerVehicle; //vehicleID/list of requests
    RequestsPerVehicle rPerVehicle;

    typedef std::map<int,TripRequest*> UnservedRequests; //requestID/request
    UnservedRequests uRequests;


  protected:
    virtual void initialize();
    virtual void finish();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    virtual std::map<int,double> getMinDistanceToPickup(StopPoint *pickupSP);
    virtual double getDistance(std::string sourceNode, int targetAddress);

  public:
    virtual StopPoint* getNextStopPoint(int vehicleID);
    virtual StopPoint* getCurrentStopPoint(int vehicleID);
    virtual void registerVehicle (Vehicle *v, std::string nodeID);

};

#endif
