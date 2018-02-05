#include <StopPoint.h>

StopPoint::StopPoint() {
    this->requestID = -1;
    this->time = 0.0;
    this->actualTime = 0.0;
    this->numberOfPassengers = 1;
    this->actualNumberOfPassengers = 0;
    this->location = -1;
    this->x_coord = -1;
    this->y_coord = -1;
    this->isPickup = false;
    this->maxDelay = 0;
}

StopPoint::StopPoint(int requestID, int location, bool isPickup, double time, double maxDelay)
{
    this->requestID = requestID;
    this->time = time;
    this->actualTime = 0.0;
    this->numberOfPassengers = 1;
    this->actualNumberOfPassengers = 0;
    this->location = location;
    this->x_coord = -1; //TODO
    this->y_coord = -1; //TODO
    this->isPickup = isPickup;
    this->maxDelay = maxDelay;
}

StopPoint::StopPoint(const StopPoint& other)
{
    copy(other);
}

void StopPoint::copy(const StopPoint& other)
{
    this->requestID = other.requestID;
    this->time = other.time;
    this->actualTime = other.actualTime;
    this->numberOfPassengers = other.numberOfPassengers;
    this->actualNumberOfPassengers = other.actualNumberOfPassengers;
    this->location = other.location;
    this->x_coord = other.x_coord;
    this->y_coord = other.y_coord;
    this->isPickup = other.isPickup;
    this->maxDelay = other.maxDelay;
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

void StopPoint::setNumberOfPassengers(int numberOfPassengers)
{
    this->numberOfPassengers = numberOfPassengers;
}

int StopPoint::getNumberOfPassengers() const
{
    return numberOfPassengers;
}

void StopPoint::setActualNumberOfPassengers(int numberOfPassengers)
{
    this->actualNumberOfPassengers = numberOfPassengers;
}

int StopPoint::getActualNumberOfPassengers() const
{
    return actualNumberOfPassengers;
}

void StopPoint::setLocation(int location)
{
    this->location = location;
}

int StopPoint::getLocation() const
{
    return location;
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

void StopPoint::setMaxDelay(double maxDelay)
{
    this->maxDelay = maxDelay;
}

double StopPoint::getMaxDelay() const
{
    return maxDelay;
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
