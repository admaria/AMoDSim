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

#ifndef STOPPOINTORDERINGPROPOSAL_H_
#define STOPPOINTORDERINGPROPOSAL_H_

#include <list>
#include "StopPoint.h"

class StopPointOrderingProposal : public cObject{
private:
    int proposalID;
    int vehicleID;
    double additionalCost;
    double actualPickupTime;
    std::list<StopPoint*> spList;
    
public:
    StopPointOrderingProposal();
    StopPointOrderingProposal(int proposalID, int vehicleID, double additionalCost, double actualPickupTime, std::list<StopPoint*> spList);
    virtual ~StopPointOrderingProposal();

    virtual void setProposalID(int proposalID);
    virtual int getProposalID() const;

    virtual void setVehicleID(int vehicleID);
    virtual int getVehicleID() const;

    virtual void setAdditionalCost(double additionalCost);
    virtual double getAdditionalCost() const;
    
    virtual void setActualPickupTime(double actualPickupTime);
    virtual double getActualPickupTime() const;

    virtual void setSpList(std::list<StopPoint*> spList);
    virtual std::list<StopPoint*> getSpList() const;
};



#endif /* STOPPOINTORDERINGPROPOSAL_H_ */
