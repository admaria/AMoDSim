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

#include "ManhattanNetworkManager.h"

Define_Module(ManhattanNetworkManager);

void ManhattanNetworkManager::initialize()
{
    cModule* parentModule = getParentModule();
    rows = parentModule->par("width");
    columns = parentModule->par("height");

    numberOfVehicles = par("numberOfVehicles");
    numberOfNodes = par("numberOfNodes");
    numberOfFixedStops = par("numberOfFixedStops");
    fixedStopPointsDistribution = par("fixedStopPointsDistribution").stdstringValue();

    if (numberOfFixedStops > 0)
    {
        if (fixedStopPointsDistribution.compare("Random") == 0)
        {
            //srand(time(NULL));
            int randN;

            for(int i=0; i<numberOfFixedStops; i++)
            {
                randN = intuniform(0, numberOfNodes -1, 6); //rand() % numberOfNodes;
                while(std::find(std::begin(fixedStops), std::end(fixedStops), randN) != std::end(fixedStops))
                    randN = intuniform(0, numberOfNodes -1, 6);
                fixedStops.push_back(randN);
                EV <<  fixedStops.back() << endl;
            }
         }

        //TODO implement different distributions

        // locate vehicles only in the fixed stop-points
        //for(int i=0; i<numberOfVehicles; i++)
          //  vehiclesPerNode[fixedStops[rand() % fixedStops.size()]]+=1;
    }

    for(int i=0; i<numberOfVehicles; i++)
        vehiclesPerNode[intuniform(0, numberOfNodes-1, 4)]+=1;

    xChannelLength = parentModule->par("xNodeDistance");
    yChannelLength = parentModule->par("yNodeDistance");

    xTravelTime = parentModule->par("xTravelTime");
    yTravelTime = parentModule->par("yTravelTime");

    xWalkingTime = parentModule->par("xWalkingTime");
    yWalkingTime = parentModule->par("yWalkingTime");

    additionalTravelTime = setAdditionalTravelTime(parentModule->par("speed"), parentModule->par("acceleration"));
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

    int ySource = srcAddr / rows;
    int yDest = dstAddr / rows;

    space_distance += abs(xSource - xDest) * xChannelLength;
    space_distance += abs(ySource - yDest) * yChannelLength;

    return space_distance;
}

int ManhattanNetworkManager::getCloserStopPoint(int srcAddr) {
    double distance = -1;
    int node = srcAddr;

    for(auto const& value: fixedStops) {
        if (value == srcAddr)
            return srcAddr;
        else
        {
            double dist = getSpaceDistance(srcAddr, value);
            if (dist < distance || distance == -1)
            {
                distance = dist;
                node = value;
            }
        }
    }
    return node;
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

    int ySource = srcAddr / rows;
    int yDest = dstAddr / rows;

    time_distance = abs(xSource - xDest) * xTravelTime;
    double yTime = abs(ySource - yDest) * yTravelTime;
    time_distance += yTime;

    if(time_distance != 0)
        time_distance+=additionalTravelTime;

    return time_distance;
}

double ManhattanNetworkManager::getWalkTimeDistance(int srcAddr, int dstAddr) {
  double time_distance = 0;

  int xSource = srcAddr % rows;
  int xDest = dstAddr % rows;

  int ySource = srcAddr / rows;
  int yDest = dstAddr / rows;

  time_distance = abs(xSource - xDest) * xWalkingTime;
  double yTime = abs(ySource - yDest) * yWalkingTime;
  time_distance += yTime;

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

int ManhattanNetworkManager::getNodeXCoord(int nodeAddr)
{
    return nodeAddr % rows;;
}

int ManhattanNetworkManager::getNodeYCoord(int nodeAddr)
{
    return nodeAddr / rows;;
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
