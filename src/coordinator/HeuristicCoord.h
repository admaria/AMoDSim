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

#ifndef HEURISTICCOORD_H_
#define HEURISTICCOORD_H_

#include <BaseCoord.h>
#include "StopPointOrderingProposal.h"

class HeuristicCoord: public BaseCoord {

private:
    std::list<StopPointOrderingProposal*> addStopPointToTrip(int vehicleID, std::list<StopPoint*> oldTrip, StopPoint* newSP);
    std::list<double> getResidualTime(std::list<StopPoint*> spl, int requestID);

protected:
    void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override;
    void handleTripRequest(TripRequest *tr) override;
    std::list<StopPoint*> eval_requestAssignment(int vehicleID, TripRequest* newTR) override;
};

#endif /* HEURISTICCOORD_H_ */
