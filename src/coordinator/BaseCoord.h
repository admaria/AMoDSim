#ifndef BASECOORD_H_
#define BASECOORD_H_

#include <omnetpp.h>
#include "TripRequest.h"
#include "Vehicle.h"
#include "VehicleState.h"
#include "AbstractNetworkManager.h"
#include <list>
#include <algorithm>

class BaseCoord : public cSimpleModule, cListener{

    protected:
        double totrequests;
        double totalAssignedRequests;
        double totalPickedupRequests;
        double totalDroppedoffRequest;
        double freeVehicles;

        int boardingTime;
        int alightingTime;
        AbstractNetworkManager* netmanager;

        //Trip related signals
        simsignal_t tripRequest;
        simsignal_t newTripAssigned;

        //Statistical signals
        simsignal_t traveledDistance;
        simsignal_t waitingTime;
        std::vector<double> waitingTimeVector;
        simsignal_t actualTripTime;
        std::vector<double> actualTripTimeVector;
        simsignal_t stretch;
        std::vector<double> stretchVector;
        simsignal_t tripDistance;
        std::vector<double> tripDistanceVector;
        simsignal_t passengersOnBoard;
        simsignal_t toDropoffRequests;
        simsignal_t toPickupRequests;
        simsignal_t requestsAssignedPerVehicle;

        simsignal_t totalRequestsPerTime;
        simsignal_t assignedRequestsPerTime;
        simsignal_t pickedupRequestsPerTime;
        simsignal_t droppedoffRequestsPerTime;
        simsignal_t freeVehiclesPerTime;

        std::map<Vehicle*, int> vehicles; //Vehicle -> node address
        std::map<int, StopPoint*> servedPickup;   //Details related to served pickup: needed to extract per-trip metrics
        std::map<int, double> rAssignedPerVehicle; //Number of requests assigned per vehicle
        std::map<int, std::map<int, VehicleState*>> statePerVehicle;

        typedef std::map<int,std::list<StopPoint*>> RequestsPerVehicle; //vehicleID/list of requests
        RequestsPerVehicle rPerVehicle;

        typedef std::map<int,TripRequest*> UnservedRequests; //requestID/request
        UnservedRequests uRequests;

        typedef std::map<int,TripRequest*> PendingRequests; //requestID/request
        PendingRequests pendingRequests;


        virtual void initialize();
        virtual void finish();

        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) = 0;
        virtual void handleTripRequest(TripRequest *tr) = 0;
        virtual bool eval_feasibility(int vehicleID, StopPoint *sp); //Evaluate if the new stop-point is feasible by a vehicle
        virtual std::list<StopPoint*> eval_requestAssignment(int vehicleID, TripRequest* newTR) = 0; //Sort the stop-points related to the specified vehicle including the new request's pickup and dropoff point, if feasible.
        virtual int minWaitingTimeAssignment (std::map<int,std::list<StopPoint*>> vehicleProposal, TripRequest* newTR); //Assign the new trip request to the vehicle which minimize the pickup waiting time

        virtual StopPoint* getRequestPickup(std::list<StopPoint*> spList, int requestID);
        virtual StopPoint* getRequestDropOff(std::list<StopPoint*> spList, int requestID);
        virtual void cleanStopPointList(std::list<StopPoint*> spList);

        virtual void updateStateElapsedTime(int vehicleID, int stateID);
        virtual int getMaxVehiclesSeats();
        virtual void collectPercentileStats(std::string sigName, std::vector<double> values);

    public:
        virtual StopPoint* getNextStopPoint(int vehicleID);
        virtual StopPoint* getCurrentStopPoint(int vehicleID);
        virtual void registerVehicle (Vehicle *v, int address);
        virtual int getLastVehicleLocation(int vehicleID);
        virtual Vehicle* getVehicleByID(int vehicleID);
        virtual bool isRequestValid(const TripRequest tr);
        virtual int countOnBoardRequests(int vehicleID);
        virtual StopPoint* getNewAssignedStopPoint(int vehicleID);
};

#endif /* BASECOORD_H_ */
