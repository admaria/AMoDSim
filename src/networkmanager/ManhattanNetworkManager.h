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

#ifndef __AMOD_SIMULATOR_MANHATTANNETWORKMANAGER_H_
#define __AMOD_SIMULATOR_MANHATTANNETWORKMANAGER_H_

#include <omnetpp.h>
#include <algorithm>
#include <AbstractNetworkManager.h>

class ManhattanNetworkManager : public AbstractNetworkManager
{
private:
    int rows;
    int columns;

    double xChannelLength;
    double yChannelLength;
    double xTravelTime;
    double yTravelTime;
    double xWalkingTime;
    double yWalkingTime;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:
    virtual double getTimeDistance(int srcAddr, int dstAddr) override;
    virtual double getWalkTimeDistance(int srcAddr, int dstAddr) override;
    virtual double getSpaceDistance(int srcAddr, int dstAddr) override;
    virtual int getCloserStopPoint(int srcAddr) override;
    virtual double getChannelLength(int nodeAddr, int gateIndex) override;
    virtual int getOutputGate(int srcAddr, int destAddr) override;
    virtual int getVehiclesPerNode(int nodeAddr) override;
    virtual bool isValidAddress(int nodeAddr) override;
    virtual int getNodeXCoord(int nodeAddress) override;
    virtual int getNodeYCoord(int nodeAddress) override;
};

#endif
