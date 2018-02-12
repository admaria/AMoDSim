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
