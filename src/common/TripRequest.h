
#ifndef TRIPREQUEST_H_
#define TRIPREQUEST_H_

#include <cobject.h>
#include "StopPoint.h"

class TripRequest: public cObject {

private:
    void copy(const TripRequest& other);

protected:
    static int nextID;
    int id;

    StopPoint *pickup;
    StopPoint *dropoff;

    int vehicleID;
    int nPassenger;
    int distanceToPickup;

public:
    TripRequest();
    virtual ~TripRequest();

    TripRequest(const TripRequest& other);

    virtual StopPoint* getPickupSP() const;
    virtual void setPickupSP(StopPoint *pickupSP);

    virtual StopPoint* getDropoffSP() const;
    virtual void setDropoffSP(StopPoint *dropoffSP);

    virtual int getID() const;
    virtual int getVehicleID() const;
    virtual void setVehicleID(int vehicleID);
    virtual int getNPassenger() const;
    virtual void setNPassenger(int nPassenger);
    virtual int getDistanceToPickup() const;
    virtual void setDistanceToPickup(int distanceToPickup);
};

#endif /* TRIPREQUEST_H_ */
