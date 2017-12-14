#ifndef BASECOORD_H_
#define BASECOORD_H_

#include <omnetpp.h>
#include "TripRequest.h"
#include "Vehicle.h"
#include <list>

class BaseCoord : public cSimpleModule, cListener{

    protected:
        int totrequests;
        int boardingTime;
        int alightingTime;

        simsignal_t tripRequest;
        simsignal_t newTripAssigned;

        //Statistical signals
        simsignal_t waitingTime;
        simsignal_t actualTripTime;
        simsignal_t tripEfficiencyRatio;
        simsignal_t tripDistance;

        std::map<Vehicle*, std::string> vehicles; //Vehicle -> nodeID
        std::map<int, StopPoint*> servedPickup; //Details related to served pickup: needed to extract per-trip metrics

        typedef std::map<int,std::list<StopPoint*>> RequestsPerVehicle; //vehicleID/list of requests
        RequestsPerVehicle rPerVehicle;

        typedef std::map<int,TripRequest*> UnservedRequests; //requestID/request
        UnservedRequests uRequests;


        virtual void initialize();
        virtual void finish();

        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) = 0;
        virtual void handleTripRequest(TripRequest *tr) = 0;
        virtual bool eval_feasibility(int vehicleID, StopPoint *sp); //Evaluate if the new stop-point is feasible by a vehicle
        virtual std::list<StopPoint*> eval_requestAssignment(int vehicleID, TripRequest* newTR) = 0; //Sort the stop-points related to the specified vehicle including the new request's pickup and dropoff point, if feasible.
        virtual int minWaitingTimeAssignment (std::map<int,std::list<StopPoint*>> vehicleProposal, TripRequest* newTR); //Assign the new trip request to the vehicle which minimize the pickup waiting time

        virtual double getDistance(std::string sourceNode, int targetAddress);
        virtual double getSpaceDistance(std::string sourceNode, int targetAddress);
        virtual StopPoint* getRequestPickup(std::list<StopPoint*> spList, int requestID);

    public:
        virtual StopPoint* getNextStopPoint(int vehicleID);
        virtual StopPoint* getCurrentStopPoint(int vehicleID);
        virtual void registerVehicle (Vehicle *v, std::string nodeID);
        virtual std::string getLastVehicleLocation(int vehicleID);

};

#endif /* BASECOORD_H_ */