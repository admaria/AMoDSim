#ifndef STOPPOINTORDERINGPROPOSAL_H_
#define STOPPOINTORDERINGPROPOSAL_H_

#include <list>
#include "StopPoint.h"

class StopPointOrderingProposal {
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