
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
 * Add a node in the geographical area
 */
void GeographicalArea::addNode(std::string nodeID)
{
    nodes.push_back(nodeID);
}

std::list<std::string> GeographicalArea::getNodes() const
{
    return nodes;
}

double GeographicalArea::getMeanTravelTime() const
{
    return meanTravelTime;
}

void GeographicalArea::setMeanTravelTime(double meanTime)
{
    meanTravelTime = meanTime;
}
