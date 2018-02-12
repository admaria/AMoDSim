/*
############################################
##              __  __       _____        ##
##        /\   |  \/  |     |  __ \       ##
##       /  \  | \  / | ___ | |  | |      ##
##      / /\ \ | |\/| |/ _ \| |  | |      ##
##     / ____ \| |  | | (_) | |__| |      ##
##    /_/    \_\_|  |_|\___/|_____/       ##
##                                        ##
## Author:                                ##
##    Andrea Di Maria                     ##
##    <andrea.dimaria90@gmail.com>        ##
############################################
*/

#include <Vehicle.h>

int Vehicle::nextID = 0;

Vehicle::Vehicle() {
    id = ++nextID;
    setName((std::to_string(id)).c_str());
    traveledDistance = 0.0;
    seats = 1;
    state = -1; //IDLE
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

double Vehicle::getTraveledDistance() const
{
    return traveledDistance;
}

void Vehicle::setTraveledDistance(double distance)
{
    this->traveledDistance = distance;
}

int Vehicle::getState() const
{
    return state;
}

void Vehicle::setState(int state)
{
    this->state = state;
}
