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

#ifndef __AMOD_SIMULATOR_NETWORKMANAGER_H_
#define __AMOD_SIMULATOR_NETWORKMANAGER_H_

#include <omnetpp.h>
#include <AbstractNetworkManager.h>

class AdaptiveNetworkManager : public AbstractNetworkManager
{
private:
    cTopology* topo;
    std::map<int,int> indexTable;
    std::map<int, std::map<int,int>> rtable;     //RoutingTable
    std::map<int, std::map<int,double>> dtable;  //Time-Distance table
    std::map<int, std::map<int,double>> sdtable; //Space-Distance table
    std::map<int, std::map<int,double>> cltable; //Channel length table

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual double timeDistanceToTarget(cTopology::Node *thisNode);
    virtual double spaceDistanceToTarget(cTopology::Node *thisNode);

    virtual void updateTables(int destAddress);

  public:
    virtual ~AdaptiveNetworkManager();

    virtual double getTimeDistance(int srcAddr, int dstAddr) override;
    virtual double getSpaceDistance(int srcAddr, int dstAddr) override;
    virtual double getChannelLength(int nodeAddr, int gateIndex) override;
    virtual int getOutputGate(int srcAddr, int destAddr) override;
    virtual int getVehiclesPerNode(int nodeAddr) override;
    virtual bool isValidAddress(int nodeAddr) override;
};

#endif
