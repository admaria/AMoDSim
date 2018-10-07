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

    this->requiredLocation = -1;
    this->walkDistance = -1;
    this->walkTime = 0.0;
    this->timeToRequiredLocation = 0.0;
    this->actualTimeToRequiredLocation = 0.0;
    this->maxDelayRequired = 0;
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
    this->requiredLocation = -1;
    this->walkDistance = -1;
    this->walkTime = 0.0;
    this->timeToRequiredLocation = 0.0;
    this->actualTimeToRequiredLocation = 0.0;
    this->maxDelayRequired = 0;
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
    this->requiredLocation = other.requiredLocation;
    this->walkDistance = other.walkDistance;
    this->walkTime = other.walkTime;
    this->timeToRequiredLocation = other.timeToRequiredLocation;
    this->actualTimeToRequiredLocation = other.actualTimeToRequiredLocation;
    this->maxDelayRequired = other.maxDelayRequired;
}


StopPoint::~StopPoint() {
}
