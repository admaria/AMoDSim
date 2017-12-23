#ifndef STOPPOINT_H_
#define STOPPOINT_H_

#include <omnetpp.h>

class StopPoint {
    private:
    void copy(const StopPoint& other);

    protected:
        int requestID;
        int location;
        int x_coord;
        int y_coord;
        std::string nodeID;
        int passenger;
        bool isPickup;
        double time;
        double actualTime;
        double maxDelay;

    public:
        StopPoint();
        StopPoint(int requestID, int location, bool isPickup, double time, double maxDelay);
        StopPoint(const StopPoint& other);
        virtual ~StopPoint();

        virtual void setRequestID(int requestID);
        virtual int getRequestID() const;

        virtual void setPassenger(int passenger);
        virtual int getPassenger() const;

        virtual void setLocation(int location);
        virtual int getLocation() const;

        virtual void setNodeID(std::string nodeID);
        virtual std::string getNodeID() const;

        virtual void setIsPickup(bool isPickup);
        virtual bool getIsPickup() const;

        virtual void setTime(double time);
        virtual double getTime() const;

        virtual void setActualTime(double actualTime);
        virtual double getActualTime() const;

        virtual void setMaxDelay(double maxDelay);
        virtual double getMaxDelay() const;

        virtual void setXcoord(int x_coord);
        virtual int getXcoord() const;

        virtual void setYcoord(int y_coord);
        virtual int getYcoord() const;
        
};

#endif /* STOPPOINT_H_ */
