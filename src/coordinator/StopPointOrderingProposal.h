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

#ifndef STOPPOINTORDERINGPROPOSAL_H_
#define STOPPOINTORDERINGPROPOSAL_H_

#include <list>
#include "StopPoint.h"

class StopPointOrderingProposal : public cObject{
private:
    int proposalID;
    int vehicleID;
    double additionalTime;
    std::list<StopPoint*> spList;
    
public:
    StopPointOrderingProposal();
    virtual ~StopPointOrderingProposal();

    virtual void setProposalID(int proposalID);
    virtual int getProposalID() const;

    virtual void setVehicleID(int vehicleID);
    virtual int getVehicleID() const;

    virtual void setAdditionalTime(double additionalTime);
    virtual double getAdditionalTime() const;
    
    virtual void setSpList(std::list<StopPoint*> spList);
    virtual std::list<StopPoint*> getSpList() const;
};



#endif /* STOPPOINTORDERINGPROPOSAL_H_ */
