#include <StopPointOrderingProposal.h>

StopPointOrderingProposal::StopPointOrderingProposal() {
    this->proposalID = -1;
    this->vehicleID = -1;
    this->additionalTime = -1.0;

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

double StopPointOrderingProposal::getAdditionalTime() const {
    return additionalTime;
}

void StopPointOrderingProposal::setAdditionalTime(double additionalTime) {
    this->additionalTime = additionalTime;
}

std::list<StopPoint*> StopPointOrderingProposal::getSpList() const {
    return spList;
}

void StopPointOrderingProposal::setSpList(std::list<StopPoint*> spList) {
    this->spList = spList;
}

