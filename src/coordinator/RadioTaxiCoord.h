#ifndef RADIOTAXICOORD_H_
#define RADIOTAXICOORD_H_

#include <BaseCoord.h>

class RadioTaxiCoord: public BaseCoord{

protected:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override;
        virtual void handleTripRequest(TripRequest *tr) override;
        virtual std::list<StopPoint*> eval_requestAssignment(int vehicleID, TripRequest* newTR) override;
};

#endif /* RADIOTAXICOORD_H_ */
