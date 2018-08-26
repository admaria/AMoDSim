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

#ifndef VEHICLESTATE_H_
#define VEHICLESTATE_H_

#include <cobject.h>

class VehicleState: public cObject {
protected:
    int stateID;
    double startingTime;
    double elapsedTime;

public:
    VehicleState();
    VehicleState(int stateID, double startingTime);
    VehicleState(int stateID);
    virtual ~VehicleState();
    
    double getElapsedTime() const;
    void setElapsedTime(double elapsedTime);
    
    int getStateID() const;
    void setStateID(int stateID);

    double getStartingTime() const;
    void setStartingTime(double startingTime);
};

#endif /* VEHICLESTATE_H_ */
