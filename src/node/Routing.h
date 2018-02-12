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

#ifndef ROUTING_H_
#define ROUTING_H_

#include <map>
#include "BaseCoord.h"
#include <omnetpp.h>
#include "AbstractNetworkManager.h"

class Routing : public cSimpleModule{
private:
    int myAddress;

    AbstractNetworkManager* netmanager;
    std::map<int,int> indexTable;

    typedef std::map<int,int> RoutingTable; // destaddr -> gateindex
    RoutingTable rtable;

    typedef std::map<int,double> DistanceTable; // destaddr -> time distance
    DistanceTable dtable;

    typedef std::map<int,double> SpaceDistanceTable; // destaddr -> space distance (in meters)
    SpaceDistanceTable sdtable;

    typedef std::map<int,double> ChannelLength; // gateIndex -> channel length
    ChannelLength cltable;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  public:
      virtual int getAddress() const;
};



#endif /* ROUTING_H_ */
