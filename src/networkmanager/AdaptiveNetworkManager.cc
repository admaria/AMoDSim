
#include "AdaptiveNetworkManager.h"

Define_Module(AdaptiveNetworkManager);

void AdaptiveNetworkManager::initialize()
{
    bool onlineRouting = par("onlineRouting").boolValue();
    numberOfVehicles = par("numberOfVehicles");
    numberOfNodes = par("numberOfNodes");

    for(int i=0; i<numberOfVehicles; i++)
        vehiclesPerNode[intuniform(0, numberOfNodes-1, 4)]+=1;

    topo = new cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back("src.node.Node");
    topo->extractByNedTypeName(nedTypes);

    for (int i=0; i<topo->getNumNodes(); i++)
    {
        int address = topo->getNode(i)->getModule()->par("address");
        indexTable[address]=i;
        if(onlineRouting)
            break;

        cTopology::Node* thisNode = NULL;
        int thisAddress;
        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        for(int j=0; j<topo->getNumNodes(); j++)
        {
            if(i==j) continue;
            thisNode = topo->getNode(j);
            thisAddress = thisNode->getModule()->par("address");
            if (thisNode->getNumPaths()==0) continue; // not connected

            cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
            int gateIndex = parentModuleGate->getIndex();

            rtable[thisAddress].insert(std::make_pair(address, gateIndex));
            dtable[thisAddress].insert(std::make_pair(address, timeDistanceToTarget(thisNode)));
            sdtable[thisAddress].insert(std::make_pair(address, spaceDistanceToTarget(thisNode)));
            cltable[thisAddress].insert(std::make_pair(gateIndex, parentModuleGate->getChannel()->par("length").doubleValue()));

        }
    }
}

AdaptiveNetworkManager::~AdaptiveNetworkManager()
{
    delete topo;
}

/**
 * Evaluate time distance from current node to target one.
 *
 * @param thisNode
 * @return
 */
double AdaptiveNetworkManager::timeDistanceToTarget(cTopology::Node *thisNode)
{
    double hopsToTarget = thisNode->getDistanceToTarget(); //get the hops to reach the target
    double timeDistance = 0.0;
    //double weight = 0.0; //Extra weight parameter

    for (int i=0; i<hopsToTarget; i++)
    {
        cTopology::LinkOut *linkOut = thisNode->getPath(0);
        timeDistance += linkOut->getLocalGate()->getChannel()->par("delay").doubleValue();
        //weight += linkOut->getWeight();

        thisNode = linkOut->getRemoteNode();
    }

    return timeDistance;
}

/**
 * Evaluate space distance from current node to target one.
 *
 * @param thisNode
 * @return
 */
double AdaptiveNetworkManager::spaceDistanceToTarget(cTopology::Node *thisNode)
{
    double distToTarget = thisNode->getDistanceToTarget(); //get the hops to reach the target
    double spaceDistance = 0.0;
    //double weight = 0.0; //Extra weight parameter

    for (int i=0; i<distToTarget; i++)
    {
        cTopology::LinkOut *linkOut = thisNode->getPath(0);
        spaceDistance += linkOut->getLocalGate()->getChannel()->par("length").doubleValue();

        //weight += linkOut->getWeight();
        thisNode = linkOut->getRemoteNode();
    }

    return spaceDistance;
}

/**
 * Return the space distance from current node to target one.
 *
 * @param dstAddress
 * @return
 */
double AdaptiveNetworkManager::getSpaceDistance(int srcAddr, int dstAddr)
{
    if(sdtable.find(srcAddr) == sdtable.end() || (sdtable[srcAddr].find(dstAddr) == sdtable[srcAddr].end()))
        updateTables(dstAddr);
    return sdtable[srcAddr].find(dstAddr)->second;
}

/**
 * Return the time distance from current node to target one.
 *
 * @param dstAddress
 * @return
 */
double AdaptiveNetworkManager::getTimeDistance(int srcAddr, int dstAddr)
{
    if(dtable.find(srcAddr) == dtable.end() || (dtable[srcAddr].find(dstAddr) == dtable[srcAddr].end()))
        updateTables(dstAddr);
    return dtable[srcAddr].find(dstAddr)->second;
}

/**
 * Return the outputGate index.
 *
 * @param dstAddress
 * @return
 */
int AdaptiveNetworkManager::getOutputGate(int srcAddr, int dstAddr)
{
    if(rtable.find(srcAddr) == rtable.end() || (rtable[srcAddr].find(dstAddr) == rtable[srcAddr].end()))
        updateTables(dstAddr);
    return rtable[srcAddr].find(dstAddr)->second;
}

/**
 * Return the length of the channel connected to the specified gate.
 *
 * @param dstAddress
 * @param gateIndex
 * @return
 */
double AdaptiveNetworkManager::getChannelLength(int nodeAddr, int gateIndex)
{
    return cltable[nodeAddr].find(gateIndex)->second;
}

/**
 * Update routing and distance tables.
 *
 * @param target address
 */
void AdaptiveNetworkManager::updateTables(int destAddress)
{

    cTopology::Node* thisNode = NULL;
    int thisAddress;
    int i = indexTable[destAddress];
    topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

    for(int j=0; j<topo->getNumNodes(); j++)
    {
        if(i==j) continue;
        thisNode = topo->getNode(j);
        thisAddress = thisNode->getModule()->par("address");
        if (thisNode->getNumPaths()==0) continue; // not connected

        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();

        rtable[thisAddress].insert(std::make_pair(destAddress, gateIndex));
        dtable[thisAddress].insert(std::make_pair(destAddress, timeDistanceToTarget(thisNode)));
        sdtable[thisAddress].insert(std::make_pair(destAddress, spaceDistanceToTarget(thisNode)));
        cltable[thisAddress].insert(std::make_pair(gateIndex, parentModuleGate->getChannel()->par("length").doubleValue()));
    }
}

/**
 * Return the vehicles started from nodeAddr.
 *
 * @param nodeAddr
 * @return
 */
int AdaptiveNetworkManager::getVehiclesPerNode(int nodeAddr)
{
    int nVehicles = 0;
    std::map<int,int>::iterator it;

    it = vehiclesPerNode.find(nodeAddr);
    if (it != vehiclesPerNode.end())
       nVehicles = it->second;

    return nVehicles;
}

/**
 * Check if the specified address is valid.
 *
 * @param dstAddress
 * @return
 */
bool AdaptiveNetworkManager::isValidAddress(int nodeAddr)
{
    if(indexTable.find(nodeAddr) != indexTable.end())
        return true;
    return false;
}

void AdaptiveNetworkManager::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
