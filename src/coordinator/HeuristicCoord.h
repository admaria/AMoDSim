
#ifndef HEURISTICCOORD_H_
#define HEURISTICCOORD_H_

#include <BaseCoord.h>

class HeuristicCoord: public BaseCoord {

private:
    virtual std::list<StopPoint*> minCostOrdering(std::list<StopPoint*> spl, StopPoint* newSP);

protected:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override;
        void handleTripRequest(TripRequest *tr) override;
        virtual std::list<StopPoint*> eval_requestAssignment(int vehicleID, TripRequest* newTR) override;
};

#endif /* HEURISTICCOORD_H_ */
