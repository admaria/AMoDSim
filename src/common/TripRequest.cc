
#include <TripRequest.h>


int TripRequest::nextID = 0;

TripRequest::TripRequest() {
    id = ++nextID;
}

TripRequest::TripRequest(const TripRequest& other)
{
    copy(other);
}

void TripRequest::copy(const TripRequest& other)
{
    this->id = other.id;
    this->pickup = other.pickup;
    this->dropoff = other.dropoff;
    this->distanceToPickup = other.distanceToPickup;
    this->vehicleID = other.vehicleID;
    this->nPassenger = other.nPassenger;
}


TripRequest::~TripRequest() {
}


int TripRequest::getID() const
{
    return id;
}

StopPoint* TripRequest::getPickupSP() const
{
    return pickup;
}

void TripRequest::setPickupSP(StopPoint *pickupSP)
{
    this->pickup = pickupSP;
}

StopPoint* TripRequest::getDropoffSP() const
{
    return dropoff;
}

void TripRequest::setDropoffSP(StopPoint *dropoffSP)
{
    this->dropoff = dropoffSP;
}

int TripRequest::getVehicleID() const
{
    return vehicleID;
}

void TripRequest::setVehicleID(int vehicleID)
{
    this->vehicleID = vehicleID;
}

int TripRequest::getNPassenger() const
{
    return nPassenger;
}

void TripRequest::setNPassenger(int nPassenger)
{
    this->nPassenger = nPassenger;
}

int TripRequest::getDistanceToPickup() const
{
    return distanceToPickup;
}

void TripRequest::setDistanceToPickup(int distanceToPickup)
{
    this->distanceToPickup = distanceToPickup;
}
