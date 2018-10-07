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

#include <omnetpp.h>
#include "TripRequest.h"
#include <map>
#include "AbstractNetworkManager.h"
#include <unordered_map>
#include "BaseCoord.h"

class TripRequestSubmitter : public cSimpleModule
{
    private:
        // configuration
        int myAddress;
        int x_coord;
        int y_coord;

        double maxSubmissionTime;
        double minTripLength;
        int destAddresses;
        int closerPickup;
        int closerPickupXcoord;
        int closerPickupYcoord;
        int walkDistanceToPickup;
        int walkTimeToPickup;

        double maxWalkTime;
        double walkTimeRatio;

        cPar *sendIATime;
        double maxDelay;
        AbstractNetworkManager *netmanager;
        BaseCoord *tcoord;

        cMessage *generatePacket;
        cMessage *emitRequest;
        long pkCounter;

        std::unordered_map<long, TripRequest *> requestsToEmit;

        // signals
        simsignal_t tripRequest;

      public:
        TripRequestSubmitter();
        virtual ~TripRequestSubmitter();

      protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual TripRequest* buildTripRequest();
};

Define_Module(TripRequestSubmitter);

TripRequestSubmitter::TripRequestSubmitter()
{
    generatePacket = NULL;
    emitRequest = NULL;
    netmanager = NULL;
    tcoord = NULL;
}

TripRequestSubmitter::~TripRequestSubmitter()
{
    cancelAndDelete(generatePacket);
    cancelAndDelete(emitRequest);
}

void TripRequestSubmitter::initialize()
{
    myAddress = par("address");
    destAddresses = par("destAddresses");
    minTripLength = par("minTripLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    maxDelay = (&par("maxDelay"))->doubleValue();
    maxSubmissionTime = par("maxSubmissionTime");
    maxWalkTime = par("maxWalkTime");
    walkTimeRatio = par("walkTimeRatio");
    closerPickup = -1;

    x_coord = getParentModule()->par("x");
    y_coord = getParentModule()->par("y");
    netmanager = check_and_cast<AbstractNetworkManager *>(getParentModule()->getParentModule()->getSubmodule("netmanager"));
    tcoord = check_and_cast<BaseCoord *>(getParentModule()->getParentModule()->getSubmodule("tcoord"));

    generatePacket = new cMessage("nextPacket");
    tripRequest = registerSignal("tripRequest");

    if (maxSubmissionTime < 0 || sendIATime->doubleValue() < maxSubmissionTime)
        scheduleAt(sendIATime->doubleValue(), generatePacket);

}


void TripRequestSubmitter::handleMessage(cMessage *msg)
{
    //EMIT a TRIP REQUEST
    if (msg == generatePacket)
    {
        TripRequest *tr = nullptr;
        double simtime = simTime().dbl();

        if (ev.isGUI()) getParentModule()->bubble("TRIP REQUEST");
        tr = buildTripRequest();

        EV << "Requiring a new trip with ID: " << tr->getID() << ", from/to: " << tr->getPickupSP()->getRequiredLocation() << "(" << tr->getPickupSP()->getLocation() << ")"<< "/" << tr->getDropoffSP()->getRequiredLocation() << "(" << tr->getDropoffSP()->getLocation() << ")" << ". I am node: " << myAddress << endl;
        EV << "Requested srcTime: " << tr->getPickupSP()->getTimeToRequiredLocation() << ". Requested dstTime: " << tr->getDropoffSP()->getTimeToRequiredLocation() << endl;
        EV << "Requested pickupTime: " << tr->getPickupSP()->getTime() << ". DropOFF required time: " << tr->getDropoffSP()->getTime() << endl;
        EV << "Delay at pickup: " << tr->getPickupSP()->getMaxDelay() << ". Delay at dropoff: " << tr->getDropoffSP()->getMaxDelay() << endl;
        EV << "WalkTime to source: " << tr->getPickupSP()->getWalkTime() << ". WalkTime to dst: " << tr->getDropoffSP()->getWalkTime() << endl;

        double walkWithService = tr->getPickupSP()->getWalkTime() + tr->getDropoffSP()->getWalkTime();
        double walkWithoutService = netmanager->getWalkTimeDistance(
                tr->getPickupSP()->getRequiredLocation(),
                tr->getDropoffSP()->getRequiredLocation());

        StopPoint *p = tr->getPickupSP();
        StopPoint *d = tr->getDropoffSP();
        if (maxWalkTime >= 0 && walkWithService > maxWalkTime)
        {
            EV << "Aborting request due to exceed in walkTime. Source: " << p->getRequiredLocation() << ", Pickup: " << p->getLocation() <<
                    ", Destination: " << d->getRequiredLocation() << ", Dropoff: " << d->getLocation() << endl;
            tcoord->abortRequest("walktime");
            delete(tr);
            return ;
        }

        if (walkTimeRatio <= 1 && walkTimeRatio >= 0 && (walkWithService/walkWithoutService) > walkTimeRatio)
        {
            EV << "Aborting request due to exceed in walkTime ratio. Source: " << p->getRequiredLocation() << ", Pickup: " << p->getLocation() <<
                    ", Destination: " << d->getRequiredLocation() << ", Dropoff: " << d->getLocation() << endl;
            tcoord->abortRequest("walkratio");
            delete(tr);
            return ;
        }

        double maxTimeAtDrop= d->getTime() + d->getMaxDelay();
        if ( maxTimeAtDrop < p->getTime())
        {
            EV << "Aborting request due to maxTimeAtDrop < minTimeAtPickup. Source: " << p->getRequiredLocation() << ", Pickup: " << p->getLocation() <<
                                ", Destination: " << d->getRequiredLocation() << ", Dropoff: " << d->getLocation() << endl;
            tcoord->abortRequest("");
            delete(tr);
            return ;
        }

        if (p->getTime() < 0)
            throw "Error";

        if (p->getTime() <= simtime)
            emit(tripRequest, tr);

        else
        {
            cMessage *ms = new cMessage("emitRequest");
            requestsToEmit[ms->getId()] = tr;
            scheduleAt(p->getTime(), ms);
        }

        //Schedule the next request
        simtime_t nextTime = simTime() + sendIATime->doubleValue();
        if (maxSubmissionTime < 0 || nextTime.dbl() < maxSubmissionTime)
        {
            EV << "Next request from node " << myAddress << "scheduled at: " << nextTime.dbl() << endl;
            scheduleAt(nextTime, generatePacket);
        }
    }

    else if (strcmp(msg->getName(), "emitRequest") == 0)
    {
        EV<< "Emitting the old scheduled request with ID: " << msg->getId() << endl;
        std::unordered_map<long, TripRequest *>::iterator it = requestsToEmit.find(msg->getId());
        if (it != requestsToEmit.end())
            emit(tripRequest, (TripRequest*)it->second);
        else
            std::cerr << "Request with ID: " <<  msg->getId() << " NOT found!" << endl;
        cancelAndDelete(msg);
    }
}

/**
 * Build a new Trip Request
 */
TripRequest* TripRequestSubmitter::buildTripRequest()
{
    TripRequest *request = new TripRequest();
    double simtime = simTime().dbl();

    // Generate a random destination address for the request
    int destAddress = intuniform(0, destAddresses-1, 3);
    while (destAddress == myAddress || netmanager->getSpaceDistance(myAddress, destAddress) < minTripLength)
        destAddress = intuniform(0, destAddresses-1, 3);

    int closerDropoff = netmanager->getCloserStopPoint(destAddress);

    if (closerPickup == -1)
    {
        closerPickup = netmanager->getCloserStopPoint(myAddress);
        closerPickupXcoord = netmanager->getNodeXCoord(closerPickup);
        closerPickupYcoord = netmanager->getNodeYCoord(closerPickup);
        walkDistanceToPickup = netmanager->getSpaceDistance(myAddress, closerPickup);
        walkTimeToPickup = netmanager->getWalkTimeDistance(myAddress, closerPickup);
    }

    // **** Pickup Stop-point **** //
    StopPoint *pickupSP = new StopPoint();
    pickupSP->setRequestID(request->getID());
    pickupSP->setIsPickup(true);
    pickupSP->setNumberOfPassengers(par("passengersPerRequest"));
    pickupSP->setLocation(closerPickup);
    pickupSP->setXCoord(closerPickupXcoord);
    pickupSP->setYCoord(closerPickupYcoord);
    pickupSP->setRequiredLocation(myAddress);
    pickupSP->setWalkTime(walkTimeToPickup);
    pickupSP->setWalkDistance(walkDistanceToPickup);
    pickupSP->setTimeToRequiredLocation(simtime);
    pickupSP->setMaxDelayRequired(maxDelay);
    pickupSP->setTime(pickupSP->getTimeToRequiredLocation() + pickupSP->getWalkTime());
    pickupSP->setMaxDelay(maxDelay);

    // **** Dropoff Stop-point **** //
    StopPoint *dropoffSP = new StopPoint();
    dropoffSP->setRequestID(request->getID());
    dropoffSP->setIsPickup(false);
    dropoffSP->setLocation(closerDropoff);
    dropoffSP->setRequiredLocation(destAddress);
    dropoffSP->setWalkTime(netmanager->getWalkTimeDistance(closerDropoff, destAddress));
    dropoffSP->setWalkDistance(netmanager->getSpaceDistance(closerDropoff, destAddress));
    dropoffSP->setTimeToRequiredLocation(simtime + netmanager->getTimeDistance(myAddress, destAddress));
    dropoffSP->setMaxDelayRequired(maxDelay);
    dropoffSP->setTime(dropoffSP->getTimeToRequiredLocation() - dropoffSP->getWalkTime());
    dropoffSP->setMaxDelay(maxDelay);

    request->setPickupSP(pickupSP);
    request->setDropoffSP(dropoffSP);

    return request;
}
