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

#ifndef __AMOD_SIMULATOR_MANHATTANNETWORKMANAGER_H_
#define __AMOD_SIMULATOR_MANHATTANNETWORKMANAGER_H_

#include <omnetpp.h>
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

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:
    virtual double getTimeDistance(int srcAddr, int dstAddr) override;
    virtual double getSpaceDistance(int srcAddr, int dstAddr) override;
    virtual double getChannelLength(int nodeAddr, int gateIndex) override;
    virtual int getOutputGate(int srcAddr, int destAddr) override;
    virtual int getVehiclesPerNode(int nodeAddr) override;
    virtual bool isValidAddress(int nodeAddr) override;
};

#endif
