#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <list>
#include <omnetpp.h>
#include "Packet_m.h"
#include "Vehicle.h"
#include "TripRequest.h"
#include "Routing.h"
#include "TripCoordinator.h"
#include <sstream>


/**
 * Node's Application level.
 */
class App : public cSimpleModule,cListener
{
  private:
    // configuration
    int myAddress;
    int x_coord;
    int y_coord;
    std::string nodePath;
    bool hasVehicle;
    int destAddresses;
    TripCoordinator *tcoord;
    cPar *sendIATime;
    cPar *maxWaitingTime;
    cPar *packetLengthBytes;
    Routing *r;

    typedef std::map<int, Vehicle*> Vehicles;
    Vehicles vehicles;

    cMessage *generatePacket;
    long pkCounter;

    // signals
    simsignal_t tripRequest;
    simsignal_t newTripAssigned;

  public:
    App();
    virtual ~App();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    virtual TripRequest* buildTripRequest();
};

Define_Module(App);


App::App()
{
    r = NULL;
    generatePacket = NULL;
}

App::~App()
{
    cancelAndDelete(generatePacket);
}


void App::initialize()
{
    myAddress = par("address");
    destAddresses = par("destAddresses");
    packetLengthBytes = &par("packetLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    maxWaitingTime = &par("maxWaitingTime");
    hasVehicle = par("hasVehicle");
    nodePath = getParentModule()->getFullPath();
    tcoord = check_and_cast<TripCoordinator *>(getParentModule()->getParentModule()->getSubmodule("tcoord"));
    x_coord = getParentModule()->par("x_distance").doubleValue() * getParentModule()->par("base_distance").doubleValue();
    y_coord = getParentModule()->par("y_distance").doubleValue() * getParentModule()->par("base_distance").doubleValue();
    r = check_and_cast<Routing *>(getParentModule()->getSubmodule("routing"));
    generatePacket = new cMessage("nextPacket");
    tripRequest = registerSignal("tripRequest");
    newTripAssigned = registerSignal("newTripAssigned");

//    pkCounter = 0;
//    WATCH(pkCounter);
//    WATCH(myAddress);

    EV << "I am node " << myAddress << ". x/y coord: " << x_coord << "/"<< y_coord << endl;

    //If the vehicle is in this node (at startup) subscribe it to "tripRequestSignal"
    if (hasVehicle)
    {
        Vehicle *v = new Vehicle();
        vehicles[v->getID()] = v;
        v->setSeats(intuniform(1, 3));
        EV << "I am node " << myAddress << ". I HAVE THE VEHICLE " << v->getID() << endl;
        tcoord->registerVehicle(v, nodePath);

        if (ev.isGUI())
            getParentModule()->getDisplayString().setTagArg("i",1,"green");

        simulation.getSystemModule()->subscribe("newTripAssigned",this);
    }

    scheduleAt(poisson(sendIATime->doubleValue()), generatePacket);

}


void App::handleMessage(cMessage *msg)
{
    //EMIT a TRIP REQUEST
    if (msg == generatePacket)
    {
        if (ev.isGUI()) getParentModule()->bubble("TRIP REQUEST");
        TripRequest *tr = buildTripRequest();

        EV << "Requiring a new trip from/to: " << tr->getPickupSP()->getNodeID() << "/" << tr->getDropoffSP()->getNodeID() << ". I am node: " << myAddress << endl;
        EV << "Requested pickupTime: " << tr->getPickupSP()->getTime() << ". DropOFF required time: " << tr->getDropoffSP()->getTime() << endl;

        emit(tripRequest, tr);

        //Schedule the next request
        simtime_t nextTime = simTime() + poisson(sendIATime->doubleValue());
        EV << "Next request from node " << myAddress << "scheduled at: " << nextTime.dbl() << endl;
        scheduleAt(nextTime, generatePacket);
    }

    //A vehicle is here
    else
    {
        Vehicle *pk = check_and_cast<Vehicle *>(msg);
        EV << "received VEHICLE " << pk->getID() << " after " << pk->getHopCount() << " hops." << endl;

        StopPoint *currentStopPoint = tcoord->getCurrentStopPoint(pk->getID());

        if (currentStopPoint != NULL && currentStopPoint->getLocation() != -1 && currentStopPoint->getIsPickup())
        {
            //This is a PICK-UP stop-point
            int waitTimeMinutes = (simTime()-currentStopPoint->getTime()).dbl() /60;
            EV << "The vehicle is here! Pickup time: " << simTime() << "; Request time: " << currentStopPoint->getTime() << "; Waiting time: " << waitTimeMinutes << "minutes." << endl;
        }

        //Ask to coordinator for next stop point
        StopPoint *nextStopPoint = tcoord->getNextStopPoint(pk->getID());
        if(nextStopPoint != NULL)
        {
            //There is another stop point for the vehicle!
            EV << "The next stop point for the vehicle " << pk->getID() << " is: " << nextStopPoint->getLocation() << endl;
            pk->setSrcAddr(myAddress);
            pk->setDestAddr(nextStopPoint->getLocation());

            send(pk,"out");
        }

        //No other stop point for the vehicle. The vehicle stay here
        else
        {
            EV << "Vehicle " << pk->getID() << " is in node " << myAddress << endl;
            vehicles[pk->getID()] = pk;
            tcoord->registerVehicle(pk, nodePath);

            if (ev.isGUI())
                getParentModule()->getDisplayString().setTagArg("i",1,"green");

            if (!simulation.getSystemModule()->isSubscribed("newTripAssigned",this))
                simulation.getSystemModule()->subscribe("newTripAssigned",this);
        }
     }

}

/**
 * Handle the signals
 */
void App::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj){


  /**
   * The coordinator has accepted a trip proposal
   */
  if(signalID == newTripAssigned)
  {
      TripRequest *pk = check_and_cast<TripRequest *>(obj);

      if(vehicles.find(pk->getVehicleID()) != vehicles.end())
      {
          //The vehicle that should serve the request is in this node
          Vehicle *veic = vehicles[pk->getVehicleID()];

          if (veic != NULL)
          {
              EV << "The proposal of vehicle: " << pk->getVehicleID() << " has been accepted for requestID:  " << pk->getID() << endl;
              veic->setSrcAddr(myAddress);
              if(myAddress != pk->getPickupSP()->getLocation())
                  veic->setDestAddr(pk->getPickupSP()->getLocation());
              else
                 veic->setDestAddr(pk->getDropoffSP()->getLocation());

              EV << "Sending Vehicle from: " << veic->getSrcAddr() << "to " << veic->getDestAddr() << endl;
              Enter_Method("send",veic,"out");
              send(veic, "out");

              vehicles.erase(veic->getID()); //the vehicle is no more in this node
              if (ev.isGUI())
                getParentModule()->getDisplayString().setTagArg("i",1,"gold");
              //if (simulation.getSystemModule()->isSubscribed("tripRequestCoord",this))
                //  simulation.getSystemModule()->unsubscribe("tripRequestCoord",this);
          }
      }
  }

}

/**
 * Build a new Trip Request
 */
TripRequest* App::buildTripRequest()
{
    TripRequest *pk = new TripRequest();
    // Generate a random destination address for the request
    int destAddress = intuniform(0, destAddresses-1);
    while (destAddress == myAddress)
        destAddress = intuniform(0, destAddresses-1);

    StopPoint *pickupSP = new StopPoint(pk->getID(), myAddress, true, simTime().dbl(), maxWaitingTime->doubleValue());
    pickupSP->setNodeID(nodePath);
    pickupSP->setXcoord(x_coord);
    pickupSP->setYcoord(y_coord);
    pickupSP->setPassenger(1);

    StopPoint *dropoffSP = new StopPoint(pk->getID(), destAddress, false, simTime().dbl() + r->getDistanceToTarget(destAddress), maxWaitingTime->doubleValue());

    //TODO to be improved
    std::stringstream ss;
    ss << "AMoD.n[" << destAddress << "]";
    std::string s = ss.str();
    dropoffSP->setNodeID(s);

    pk->setPickupSP(pickupSP);
    pk->setDropoffSP(dropoffSP);
    pk->setNPassenger(1);

    return pk;
}
