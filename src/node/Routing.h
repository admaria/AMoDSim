/*
 * Routing.h
 *
 *  Created on: Nov 3, 2017
 *      Author: andrea
 */

#ifndef ROUTING_H_
#define ROUTING_H_

#include <map>
#include <omnetpp.h>

class Routing : public cSimpleModule{
private:
    int myAddress;
    double channelTime;
    cTopology *topo;

    typedef std::map<int,int> RoutingTable; // destaddr -> gateindex
    RoutingTable rtable;

    typedef std::map<int,double> DistanceTable; // destaddr -> time distance
    DistanceTable dtable;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual double timeDistanceToTarget(cTopology::Node *thisNode);

  public:
      virtual int getAddress() const;
      virtual double getDistanceToTarget(int dstAddr);
};



#endif /* ROUTING_H_ */
