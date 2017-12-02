
#include <GeographicalArea.h>

GeographicalArea::GeographicalArea()
{
    meanTravelTime = 0.0;
}

GeographicalArea::GeographicalArea(char *hashValue)
{
    meanTravelTime = 0.0;
    hash=hashValue;
}

GeographicalArea::~GeographicalArea()
{

}

/**
 * Add a node in the geographical area.
 * 
 * @param nodeID
 */
void GeographicalArea::addNode(std::string nodeID)
{
    nodes.push_back(nodeID);
}

/**
 * Get the nodes located in the Geographical area.
 * 
 * @return 
 */
std::list<std::string> GeographicalArea::getNodes() const
{
    return nodes;
}

/**
 * Get the mean time needed to travel through the geographical area.
 * 
 * @return 
 */
double GeographicalArea::getMeanTravelTime() const
{
    return meanTravelTime;
}

/**
 * Set the mean time needed to travel through the geographical area.
 * 
 * @param meanTime
 */
void GeographicalArea::setMeanTravelTime(double meanTime)
{
    meanTravelTime = meanTime;
}
