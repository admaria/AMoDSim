/*
 * AbstractNetworkManager.h
 *
 *  Created on: Jan 18, 2018
 *      Author: andrea
 */

#ifndef ABSTRACTNETWORKMANAGER_H_
#define ABSTRACTNETWORKMANAGER_H_


class AbstractNetworkManager : public cSimpleModule{

protected:
    virtual void initialize() = 0;
    virtual void handleMessage(cMessage *msg) = 0;

  public:
    virtual double getTimeDistance(int srcAddr, int dstAddr)=0;
    virtual double getSpaceDistance(int srcAddr, int dstAddr)=0;
    virtual double getChannelLength(int nodeAddr, int gateIndex)=0;
    virtual int getOutputGate(int srcAddr, int destAddr)=0;

};


#endif /* ABSTRACTNETWORKMANAGER_H_ */
