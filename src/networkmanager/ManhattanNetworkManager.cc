#include "ManhattanNetworkManager.h"

Define_Module(ManhattanNetworkManager);

void ManhattanNetworkManager::initialize()
{
    rows = getParentModule()->par("width");
    columns = getParentModule()->par("height");

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
