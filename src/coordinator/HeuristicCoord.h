
#ifndef HEURISTICCOORD_H_
#define HEURISTICCOORD_H_

#include <BaseCoord.h>
#include "StopPointOrderingProposal.h"

class HeuristicCoord: public BaseCoord {

private:
    virtual std::list<StopPointOrderingProposal*> addStopPointToTrip(int vehicleID, std::list<StopPoint*> oldTrip, StopPoint* newSP);
    virtual std::list<double> getResidualTime(std::list<StopPoint*> spl, int requestID);

protected:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override;
        void handleTripRequest(TripRequest *tr) override;
        virtual std::list<StopPoint*> eval_requestAssignment(int vehicleID, TripRequest* newTR) override;
};

#endif /* HEURISTICCOORD_H_ */
