#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <list>
#include <omnetpp.h>
#include "Packet_m.h"
#include "Vehicle.h"
#include "TripRequest.h"
#include "BaseCoord.h"
#include "AbstractNetworkManager.h"
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
    int numberOfVehicles;
    int seatsPerVehicle;
    BaseCoord *tcoord;
    AbstractNetworkManager *netmanager;

    // signals
    simsignal_t newTripAssigned;

  public:
    App();
    virtual ~App();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double vehicleID);
};

Define_Module(App);


App::App()
{
    tcoord = NULL;
}

App::~App()
{
}


void App::initialize()
{
    myAddress = par("address");
    seatsPerVehicle = par("seatsPerVehicle");

    tcoord = check_and_cast<BaseCoord *>(getParentModule()->getParentModule()->getSubmodule("tcoord"));
    netmanager = check_and_cast<AbstractNetworkManager *>(getParentModule()->getParentModule()->getSubmodule("netmanager"));
    numberOfVehicles = netmanager->getVehiclesPerNode(myAddress);

    x_coord = getParentModule()->par("x_distance").doubleValue() * getParentModule()->par("xBase_distance").doubleValue();
    y_coord = getParentModule()->par("y_distance").doubleValue() * getParentModule()->par("yBase_distance").doubleValue();

    newTripAssigned = registerSignal("newTripAssigned");

    EV << "I am node " << myAddress << ". x/y coord(meters): " << x_coord << "/"<< y_coord << endl;

    //If the vehicle is in this node (at startup) subscribe it to "tripRequestSignal"
    if (numberOfVehicles > 0)
    {
        for(int i=0; i<numberOfVehicles; i++)
        {
            Vehicle *v = new Vehicle();
            v->setSeats(seatsPerVehicle);
            EV << "I am node " << myAddress << ". I HAVE THE VEHICLE " << v->getID() << ". It has " << v->getSeats() << " seats." << endl;
            tcoord->registerVehicle(v, myAddress);
        }

        if (ev.isGUI())
            getParentModule()->getDisplayString().setTagArg("i",1,"green");

        //When the coordinator assign a new request to a vehicle, local node will be notified
        simulation.getSystemModule()->subscribe("newTripAssigned",this);
    }

}


void App::handleMessage(cMessage *msg)
{
    Vehicle *vehicle = NULL;

    try{
        //A vehicle is here
        vehicle = check_and_cast<Vehicle *>(msg);
    }catch (cRuntimeError e) {
        EV << "Can not handle received message! Ignoring..." << endl;
        return ;
    }

    EV << "received VEHICLE " << vehicle->getID() << " after " << vehicle->getHopCount() << " hops." << endl;
    StopPoint *currentStopPoint = tcoord->getCurrentStopPoint(vehicle->getID());

    if (currentStopPoint != NULL && currentStopPoint->getLocation() != -1 && currentStopPoint->getIsPickup())
    {
        //This is a PICK-UP stop-point
        double waitTimeMinutes = (simTime().dbl() - currentStopPoint->getTime()) / 60;
        EV << "The vehicle is here! Pickup time: " << simTime() << "; Request time: " << currentStopPoint->getTime() << "; Waiting time: " << waitTimeMinutes << "minutes." << endl;
    }

    //Ask to coordinator for next stop point
    StopPoint *nextStopPoint = tcoord->getNextStopPoint(vehicle->getID());
    if(nextStopPoint != NULL)
    {
        //There is another stop point for the vehicle!
        EV << "The next stop point for the vehicle " << vehicle->getID() << " is: " << nextStopPoint->getLocation() << endl;
        vehicle->setSrcAddr(myAddress);
        vehicle->setDestAddr(nextStopPoint->getLocation());

        send(vehicle,"out");
    }

    //No other stop point for the vehicle. The vehicle stay here
    else
    {
        EV << "Vehicle " << vehicle->getID() << " is in node " << myAddress << endl;
        tcoord->registerVehicle(vehicle, myAddress);

        if (ev.isGUI())
            getParentModule()->getDisplayString().setTagArg("i",1,"green");

        if (!simulation.getSystemModule()->isSubscribed("newTripAssigned",this))
            simulation.getSystemModule()->subscribe("newTripAssigned",this);
    }


}

/**
 * Handle an Omnet signal.
 * 
 * @param source
 * @param signalID
 * @param obj
 */
void App::receiveSignal(cComponent *source, simsignal_t signalID, double vehicleID){


  /**
   * The coordinator has accepted a trip proposal
   */
  if(signalID == newTripAssigned)
  {

      if(tcoord->getLastVehicleLocation(vehicleID) == myAddress)
      {
          //The vehicle that should serve the request is in this node
          Vehicle *veic = tcoord->getVehicleByID(vehicleID);

          if (veic != NULL)
          {
              StopPoint* sp =tcoord->getNewAssignedStopPoint(veic->getID());
              EV << "The proposal of vehicle: " << veic->getID() << " has been accepted for requestID:  " << sp->getRequestID() << endl;
              veic->setSrcAddr(myAddress);
              veic->setDestAddr(sp->getLocation());

              EV << "Sending Vehicle from: " << veic->getSrcAddr() << "to " << veic->getDestAddr() << endl;
              Enter_Method("send",veic,"out");
              send(veic, "out");

              if (ev.isGUI())
                getParentModule()->getDisplayString().setTagArg("i",1,"gold");
              //if (simulation.getSystemModule()->isSubscribed("tripRequestCoord",this))
                //  simulation.getSystemModule()->unsubscribe("tripRequestCoord",this);
          }
      }
  }

}
