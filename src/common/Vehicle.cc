#include <Vehicle.h>

int Vehicle::nextID = 0;

Vehicle::Vehicle() {
    id = ++nextID;
    setName((std::to_string(id)).c_str());
    traveledDistance = 0;
    seats = 1;
}

Vehicle::~Vehicle() {
}

int Vehicle::getID() const
{
    return id;
}

int Vehicle::getSeats() const
{
    return seats;
}

void Vehicle::setSeats(int seats)
{
    this->seats = seats;
}

int Vehicle::getTraveledDistance() const
{
    return traveledDistance;
}

void Vehicle::setTraveledDistance(int distance)
{
    this->traveledDistance = distance;
}


