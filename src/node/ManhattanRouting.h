#ifndef __AMOD_SIMULATOR_MANHATTANROUTING_H_
#define __AMOD_SIMULATOR_MANHATTANROUTING_H_

#include <omnetpp.h>

class ManhattanRouting : public cSimpleModule
{
private:
    int myAddress;
    int myX;
    int myY;
    int rows;
    int columns;
    double xChannelLength;
    double yChannelLength;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
