#include "ManhattanNetworkManager.h"

Define_Module(ManhattanNetworkManager);

void ManhattanNetworkManager::initialize()
{
    rows = getParentModule()->par("width");
    columns = getParentModule()->par("height");

    numberOfVehicles = par("numberOfVehicles");
    numberOfNodes = par("numberOfNodes");

    for(int i=0; i<numberOfVehicles; i++)
        vehiclesPerNode[intuniform(0, numberOfNodes-1, 3)]+=1;

    xChannelLength = getParentModule()->par("xNodeDistance");
    yChannelLength = getParentModule()->par("yNodeDistance");

    xTravelTime = getParentModule()->par("xTravelTime");
    yTravelTime = getParentModule()->par("yTravelTime");
}

/**
 * Return the space distance from current node to target one.
 *
 * @param srcAddr
 * @param dstAddress
 * @return
 */
double ManhattanNetworkManager::getSpaceDistance(int srcAddr, int dstAddr)
{
    double space_distance = 0;

    int xSource = srcAddr % rows;
    int xDest = dstAddr % rows;

    int ySource = srcAddr / columns;
    int yDest = dstAddr / columns;

    space_distance += abs(xSource - xDest) * xChannelLength;
    space_distance += abs(ySource - yDest) * yChannelLength;

    return space_distance;
}

/**
 * Return the time distance from current node to target one.
 *
 * @param dstAddress
 * @return
 */
double ManhattanNetworkManager::getTimeDistance(int srcAddr, int dstAddr)
{
    double time_distance = 0;

    int xSource = srcAddr % rows;
    int xDest = dstAddr % rows;

    int ySource = srcAddr / columns;
    int yDest = dstAddr / columns;

    time_distance += abs(xSource - xDest) * xTravelTime;
    time_distance += abs(ySource - yDest) * yTravelTime;

    return time_distance;
}

/**
 * Return the vehicles started from nodeAddr.
 *
 * @param nodeAddr
 * @return
 */
int ManhattanNetworkManager::getVehiclesPerNode(int nodeAddr)
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
bool ManhattanNetworkManager::isValidAddress(int nodeAddr)
{
    if(nodeAddr >= 0 && nodeAddr < numberOfNodes)
        return true;
    return false;
}

/**
 * Return the outputGate index.
 *
 * @param dstAddress
 * @return
 */
int ManhattanNetworkManager::getOutputGate(int srcAddr, int dstAddr)
{
   return -1;
}

/**
 * Return the length of the channel connected to the specified gate.
 *
 * @param dstAddress
 * @param gateIndex
 * @return
 */
double ManhattanNetworkManager::getChannelLength(int nodeAddr, int gateIndex)
{
    return -1;
}

void ManhattanNetworkManager::handleMessage(cMessage *msg)
{

}
