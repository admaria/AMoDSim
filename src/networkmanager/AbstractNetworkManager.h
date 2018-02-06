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
    std::map<int,int> vehiclesPerNode;  //Number of vehicles per node at simulation start
    int numberOfVehicles;               //Number of vehicles in the network
    int numberOfNodes;                  //Number of crossroads(nodes) in the network

    virtual void initialize() = 0;
    virtual void handleMessage(cMessage *msg) = 0;

  public:
    virtual double getTimeDistance(int srcAddr, int dstAddr)=0;      //Get the time needed to go from srcAddr to dstAddr
    virtual double getSpaceDistance(int srcAddr, int dstAddr)=0;     //Get the space-distance from srcAddr to dstAddr
    virtual double getChannelLength(int nodeAddr, int gateIndex)=0;  //Get the length of the channel connected to the specified gate
    virtual int getOutputGate(int srcAddr, int destAddr)=0;          //Get the index of the gate where send the packet to reach the destAddr
    virtual int getVehiclesPerNode(int nodeAddr)=0;                  //Get the number of vehicles located in the node at simulation start
    virtual bool isValidAddress(int nodeAddr)=0;                     //Check if the specified address is valid
    virtual int getNumberOfVehicles(){return numberOfVehicles;}
};


#endif /* ABSTRACTNETWORKMANAGER_H_ */
