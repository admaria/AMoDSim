
#ifndef __AMOD_SIMULATOR_NETWORKMANAGER_H_
#define __AMOD_SIMULATOR_NETWORKMANAGER_H_

#include <omnetpp.h>

class NetworkManager : public cSimpleModule
{
private:
    cTopology* topo;
    std::map<int,int> indexTable;
    std::map<int, std::map<int,int>> rtable;     //RoutingTable
    std::map<int, std::map<int,double>> dtable;  //Time-Distance table
    std::map<int, std::map<int,double>> sdtable; //Space-Distance table
    std::map<int, std::map<int,double>> cltable; //Channel length table

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual double timeDistanceToTarget(cTopology::Node *thisNode);
    virtual double spaceDistanceToTarget(cTopology::Node *thisNode);

    virtual void updateTables(int destAddress);

  public:
    virtual double getTimeDistance(int srcAddr, int dstAddr);
    virtual double getSpaceDistance(int srcAddr, int dstAddr);
    virtual double getChannelLength(int nodeAddr, int gateIndex);
    virtual int getOutputGate(int srcAddr, int destAddr);
};

#endif
