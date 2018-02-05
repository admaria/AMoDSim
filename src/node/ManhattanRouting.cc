
#include "ManhattanRouting.h"
#include "Vehicle.h"

Define_Module(ManhattanRouting);

void ManhattanRouting::initialize()
{
    myAddress = getParentModule()->par("address");
    myX = getParentModule()->par("x");
    myY = getParentModule()->par("y");
    rows = getParentModule()->getParentModule()->par("width");
    columns = getParentModule()->getParentModule()->par("height");

    xChannelLength = getParentModule()->getParentModule()->par("xNodeDistance");
    yChannelLength = getParentModule()->getParentModule()->par("yNodeDistance");

    EV << "I am node " << myAddress << ". My X/Y are: " << myX << "/" << myY << endl;
}

void ManhattanRouting::handleMessage(cMessage *msg)
{
    Vehicle *pk = check_and_cast<Vehicle *>(msg);
    int destAddr = pk->getDestAddr();

    //If this node is the destination, forward the vehicle to the application level
    if (destAddr == myAddress)
    {
        EV << "Vehicle arrived in the stop point " << myAddress << ". Traveled distance: " << pk->getTraveledDistance() << endl;
        send(pk, "localOut");
        return;
    }


    int distance;
    int outGateIndex;
    int destX = pk->getDestAddr() % rows;
    int destY = pk->getDestAddr() / rows;

    if(myX < destX)
    {
        outGateIndex = 2; //right
        distance = xChannelLength;
    }
    else
        if(myX > destX)
        {
            outGateIndex = 3; //left
            distance = xChannelLength;
        }
    else
        if(myY < destY)
        {
            outGateIndex = 0; //sud
            distance = yChannelLength;
        }
        else
        {
            outGateIndex = 1; //north
            distance = yChannelLength;
        }

    pk->setHopCount(pk->getHopCount()+1);
    pk->setTraveledDistance(pk->getTraveledDistance() + distance);

    //send the vehicle to the next node
    send(pk, "out", outGateIndex);
}
