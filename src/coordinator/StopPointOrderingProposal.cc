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

#include <StopPointOrderingProposal.h>

StopPointOrderingProposal::StopPointOrderingProposal() {
    this->proposalID = -1;
    this->vehicleID = -1;
    this->additionalCost = -1.0;
    this->actualPickupTime = -1.0;
}

StopPointOrderingProposal::StopPointOrderingProposal(int proposalID, int vehicleID, double additionalCost, double actualPickupTime, std::list<StopPoint*> spList) {
    this->proposalID = proposalID;
    this->vehicleID = vehicleID;
    this->additionalCost = additionalCost;
    this->actualPickupTime = actualPickupTime;
    this->spList = spList;
}

StopPointOrderingProposal::~StopPointOrderingProposal() {
    for(auto &it:spList) delete it;

    spList.clear();
}

int StopPointOrderingProposal::getProposalID() const {
    return proposalID;
}

void StopPointOrderingProposal::setProposalID(int proposalID) {
    this->proposalID = proposalID;
}

int StopPointOrderingProposal::getVehicleID() const {
    return vehicleID;
}

void StopPointOrderingProposal::setVehicleID(int vehicleID) {
    this->vehicleID = vehicleID;
}

double StopPointOrderingProposal::getAdditionalCost() const {
    return additionalCost;
}

void StopPointOrderingProposal::setAdditionalCost(double additionalCost) {
    this->additionalCost = additionalCost;
}

double StopPointOrderingProposal::getActualPickupTime() const {
    return actualPickupTime;
}

void StopPointOrderingProposal::setActualPickupTime(double actualPickupTime) {
    this->actualPickupTime = actualPickupTime;
}

std::list<StopPoint*> StopPointOrderingProposal::getSpList() const {
    return spList;
}

void StopPointOrderingProposal::setSpList(std::list<StopPoint*> spList) {
    this->spList = spList;
}

