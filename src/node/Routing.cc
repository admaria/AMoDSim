#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <map>
#include <omnetpp.h>
#include "Packet_m.h"
#include "Vehicle.h"
#include "Routing.h"

void Routing::initialize()
{
    myAddress = getParentModule()->par("address");
    channelTime = getParentModule()->getParentModule()->par("channelTime");
    channelLength = getParentModule()->getParentModule()->par("nodeDistance");

    //
    // Brute force approach -- every node does topology discovery on its own,
    // and finds routes to all other nodes independently, at the beginning
    // of the simulation. This could be improved: (1) central routing database,
    // (2) on-demand route calculation
    //
    topo = new cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back(getParentModule()->getNedTypeName());
    topo->extractByNedTypeName(nedTypes);

    cTopology::Node *thisNode = topo->getNodeFor(getParentModule());

    // find and store next hops
    for (int i=0; i<topo->getNumNodes(); i++)
    {
        if (topo->getNode(i)==thisNode) continue; // skip ourselves
        //topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        topo->calculateWeightedSingleShortestPathsTo(topo->getNode(i));

        if (thisNode->getNumPaths()==0) continue; // not connected

        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();
        int address = topo->getNode(i)->getModule()->par("address");
        rtable[address] = gateIndex;
        dtable[address] = timeDistanceToTarget(thisNode);
        sdtable[address] = spaceDistanceToTarget(thisNode);
    }

    delete topo;
}

Define_Module(Routing);

void Routing::handleMessage(cMessage *msg)
{
    Vehicle *pk = check_and_cast<Vehicle *>(msg);
    int destAddr = pk->getDestAddr();

    //If this node is the destination, forward the vehicle to the application level
    if (destAddr == myAddress)
    {
        send(pk, "localOut");
        return;
    }

    RoutingTable::iterator it = rtable.find(destAddr);
    //The destination address is not reachable
    if (it==rtable.end())
    {
        delete pk;
        return;
    }

    int outGateIndex = (*it).second;
    pk->setHopCount(pk->getHopCount()+1);
    pk->setTraveledDistance(pk->getTraveledDistance() + channelLength);

    //send the vehicle to the next node
    send(pk, "out", outGateIndex);
}

/**
 * Return the time distance from current node to target one.
 * 
 * @param dstAddress
 * @return 
 */
double Routing::getDistanceToTarget(int dstAddress){
    return dtable.find(dstAddress)->second;
}

/**
 * Return the space distance from current node to target one.
 *
 * @param dstAddress
 * @return
 */
double Routing::getSpaceDistanceToTarget(int dstAddress){
    return sdtable.find(dstAddress)->second;
}

/**
 * Evaluate time distance from current node to target one.
 * 
 * @param thisNode
 * @return 
 */
double Routing::timeDistanceToTarget(cTopology::Node *thisNode)
{
    double distToTarget = thisNode->getDistanceToTarget(); //get the hops to reach the target
    double weight = 0.0;

    for (int i=0; i<distToTarget; i++)
    {
        cTopology::LinkOut *linkOut = thisNode->getPath(0);
        weight += linkOut->getWeight();
        thisNode = linkOut->getRemoteNode();
    }

    return weight*channelTime;
}

/**
 * Evaluate space distance from current node to target one.
 *
 * @param thisNode
 * @return
 */
double Routing::spaceDistanceToTarget(cTopology::Node *thisNode)
{
    double distToTarget = thisNode->getDistanceToTarget(); //get the hops to reach the target
    double weight = 0.0;

    for (int i=0; i<distToTarget; i++)
    {
        cTopology::LinkOut *linkOut = thisNode->getPath(0);
        weight += linkOut->getWeight();
        thisNode = linkOut->getRemoteNode();
    }

    return weight*channelLength;
}

/**
 * Get the current node address.
 */
int Routing::getAddress() const
{
    return myAddress;
}
