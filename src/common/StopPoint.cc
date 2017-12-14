#include <StopPoint.h>

StopPoint::StopPoint() {
    this->requestID = -1;
    this->time = 0.0;
    this->actualTime = 0.0;
    this->nodeID = "";
    this->passenger = 0;
    this->location = -1;
    this->x_coord = -1;
    this->y_coord = -1;
    this->isPickup = false;
    this->maxWaitingTime = 0;
}

StopPoint::StopPoint(int requestID, int location, bool isPickup, double time, double maxWaitingTime)
{
    this->requestID = requestID;
    this->location = location;
    this->nodeID = "";
    this->isPickup = isPickup;
    this->time = time;
    this->actualTime = 0.0;
    this->passenger = 0;
    this->maxWaitingTime = maxWaitingTime;
    this->x_coord = -1; //TODO
    this->y_coord = -1; //TODO
}

StopPoint::StopPoint(const StopPoint& other)
{
    copy(other);
}

void StopPoint::copy(const StopPoint& other)
{
    this->requestID = other.requestID;
    this->location = other.location;
    this->nodeID = other.nodeID;
    this->isPickup = other.isPickup;
    this->time = other.time;
    this->actualTime = other.actualTime;
    this->passenger = other.passenger;
    this->maxWaitingTime = other.maxWaitingTime;
    this->x_coord = other.x_coord;
    this->y_coord = other.y_coord;
}


StopPoint::~StopPoint() {
}


void StopPoint::setRequestID(int requestID)
{
    this->requestID = requestID;
}

int StopPoint::getRequestID() const
{
    return requestID;
}

void StopPoint::setPassenger(int passenger)
{
    this->passenger = passenger;
}

int StopPoint::getPassenger() const
{
    return passenger;
}

void StopPoint::setLocation(int location)
{
    this->location = location;
}

int StopPoint::getLocation() const
{
    return location;
}

void StopPoint::setNodeID(std::string nodeID)
{
    this->nodeID = nodeID;
}

std::string StopPoint::getNodeID() const
{
    return nodeID;
}

void StopPoint::setIsPickup(bool isPickup)
{
    this->isPickup = isPickup;
}

bool StopPoint::getIsPickup() const
{
    return isPickup;
}

void StopPoint::setTime(double time)
{
    this->time = time;
}

double StopPoint::getTime() const
{
    return time;
}

void StopPoint::setActualTime(double actualTime)
{
    this->actualTime = actualTime;
}

double StopPoint::getActualTime() const
{
    return actualTime;
}

void StopPoint::setMaxWaitingTime(double maxWaitingTime)
{
    this->maxWaitingTime = maxWaitingTime;
}

double StopPoint::getMaxWaitingTime() const
{
    return maxWaitingTime;
}

void StopPoint::setXcoord(int x_coord)
{
    this->x_coord = x_coord;
}

int StopPoint::getXcoord() const
{
    return x_coord;
}

void StopPoint::setYcoord(int y_coord)
{
    this->y_coord = y_coord;
}

int StopPoint::getYcoord() const
{
    return y_coord;
}
