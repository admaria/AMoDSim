
#include <omnetpp.h>
#include "TripRequest.h"
#include "AbstractNetworkManager.h"

class TripRequestSubmitter : public cSimpleModule
{
    private:
        // configuration
        int myAddress;
        int x_coord;
        int y_coord;

        double maxSubmissionTime;
        int destAddresses;

        cPar *sendIATime;
        cPar *maxDelay;
        AbstractNetworkManager *netmanager;

        cMessage *generatePacket;
        long pkCounter;

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
    netmanager = NULL;
}

TripRequestSubmitter::~TripRequestSubmitter()
{
    cancelAndDelete(generatePacket);
}

void TripRequestSubmitter::initialize()
{
    myAddress = par("address");
    destAddresses = par("destAddresses");
    sendIATime = &par("sendIaTime");  // volatile parameter
    maxDelay = &par("maxDelay");
    maxSubmissionTime = par("maxSubmissionTime");

    x_coord = getParentModule()->par("x_distance").doubleValue() * getParentModule()->par("xBase_distance").doubleValue();
    y_coord = getParentModule()->par("y_distance").doubleValue() * getParentModule()->par("yBase_distance").doubleValue();
    netmanager = check_and_cast<AbstractNetworkManager *>(getParentModule()->getParentModule()->getSubmodule("netmanager"));

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

        if (ev.isGUI()) getParentModule()->bubble("TRIP REQUEST");
        tr = buildTripRequest();

        EV << "Requiring a new trip from/to: " << tr->getPickupSP()->getLocation() << "/" << tr->getDropoffSP()->getLocation() << ". I am node: " << myAddress << endl;
        EV << "Requested pickupTime: " << tr->getPickupSP()->getTime() << ". DropOFF required time: " << tr->getDropoffSP()->getTime() << ". Passengers: " << tr->getPickupSP()->getNumberOfPassengers() << endl;

        emit(tripRequest, tr);

        //Schedule the next request
        simtime_t nextTime = simTime() + sendIATime->doubleValue();
        if (maxSubmissionTime < 0 || nextTime.dbl() < maxSubmissionTime)
        {
            EV << "Next request from node " << myAddress << "scheduled at: " << nextTime.dbl() << endl;
            scheduleAt(nextTime, generatePacket);
        }
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
    int destAddress = myAddress;
    while (destAddress == myAddress)
        destAddress = intuniform(0, destAddresses-1, 3);

    StopPoint *pickupSP = new StopPoint(request->getID(), myAddress, true, simtime, maxDelay->doubleValue());
    pickupSP->setXcoord(x_coord);
    pickupSP->setYcoord(y_coord);
    pickupSP->setNumberOfPassengers(par("passengersPerRequest"));

    StopPoint *dropoffSP = new StopPoint(request->getID(), destAddress, false, simtime + netmanager->getTimeDistance(myAddress, destAddress), maxDelay->doubleValue());

    request->setPickupSP(pickupSP);
    request->setDropoffSP(dropoffSP);

    return request;
}
