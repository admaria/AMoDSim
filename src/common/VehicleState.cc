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

#include <VehicleState.h>

VehicleState::VehicleState() {
    this->stateID = -1;
    this->elapsedTime = this->startingTime = 0.0;
}

VehicleState::VehicleState(int stateID, double startingTime) {
    this->stateID = stateID;
    this->startingTime = startingTime;
    this->elapsedTime = 0.0;
}

VehicleState::VehicleState(int stateID) {
    this->stateID = stateID;
    this->elapsedTime = this->startingTime = 0.0;
}


double VehicleState::getElapsedTime() const {
    return elapsedTime;
}

void VehicleState::setElapsedTime(double elapsedTime) {
    this->elapsedTime = elapsedTime;
}

int VehicleState::getStateID() const {
    return stateID;
}

void VehicleState::setStateID(int stateID) {
    this->stateID = stateID;
}

double VehicleState::getStartingTime() const {
    return startingTime;
}

void VehicleState::setStartingTime(double startingTime) {
    this->startingTime = startingTime;
}

VehicleState::~VehicleState() {
}

