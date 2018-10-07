/*
########################################################
##           __  __       _____   _____ _             ##
##     /\   |  \/  |     |  __ \ / ____(_)            ##
##    /  \  | \  / | ___ | |  | | (___  _ _ __ ___    ##
##   / /\ \ | |\/| |/ _ \| |  | |\___ \| | '_ ` _ \   ##
##  / ____ \| |  | | (_) | |__| |____) | | | | | | |  ##
## /_/    \_\_|  |_|\___/|_____/|_____/|_|_| |_| |_|  ##
##                                                    ##
## Author:                                            ##
##    Andrea Di Maria                                 ##
##    <andrea.dimaria90@gmail.com>                    ##
########################################################
*/

#ifndef STOPPOINT_H_
#define STOPPOINT_H_

#include <omnetpp.h>

class StopPoint : public cObject{
    private:
    void copy(const StopPoint& other);

    protected:
        int requestID;

        int location;                   // the pickup or dropoff location
        int x_coord;                    // the pickup or dropoff x coordinate
        int y_coord;                    // the pickup or dropoff y coordinate

        int requiredLocation;           // the required pickup or dropoff location
        double walkDistance;               // the walk distance from requiredLocation to location (meters)
        double walkTime;                   // the walk time distance from requiredLocation to location (meters)

        int numberOfPassengers;
        int actualNumberOfPassengers;
        bool isPickup;

        double timeToRequiredLocation;  // the ideal time to reach the required location
        double time;                    // the ideal time to reach the fixed stop-point location
        double actualTimeToRequiredLocation; // the actual time at the required location
        double actualTime;              // the actual time at the fixed stop-point location
        double maxDelay;                // the maximum delay allowed over the time
        double maxDelayRequired;        // the maximum delay allowed over the timeToRequiredLocation


    public:
        StopPoint();
        StopPoint(int requestID, int location, bool isPickup, double time, double maxDelay);
        StopPoint(const StopPoint& other);
        virtual ~StopPoint();

        int getActualNumberOfPassengers() const {
            return actualNumberOfPassengers;
        }

        void setActualNumberOfPassengers(int actualNumberOfPassengers) {
            this->actualNumberOfPassengers = actualNumberOfPassengers;
        }

        double getActualTime() const {
            return actualTime;
        }

        void setActualTime(double actualTime) {
            this->actualTime = actualTime;
        }

        double getActualTimeToRequiredLocation() const {
            return actualTimeToRequiredLocation;
        }

        void setActualTimeToRequiredLocation(double actualTimeToRequiredLocation) {
            this->actualTimeToRequiredLocation = actualTimeToRequiredLocation;
        }

        bool getIsPickup() const {
            return isPickup;
        }

        void setIsPickup(bool isPickup) {
            this->isPickup = isPickup;
        }

        int getLocation() const {
            return location;
        }

        void setLocation(int location) {
            this->location = location;
        }

        double getMaxDelay() const {
            return maxDelay;
        }

        void setMaxDelay(double maxDelay) {
            this->maxDelay = maxDelay;
        }


        int getNumberOfPassengers() const {
            return numberOfPassengers;
        }

        void setNumberOfPassengers(int numberOfPassengers) {
            this->numberOfPassengers = numberOfPassengers;
        }

        int getRequestID() const {
            return requestID;
        }

        void setRequestID(int requestId) {
            requestID = requestId;
        }

        int getRequiredLocation() const {
            return requiredLocation;
        }

        void setRequiredLocation(int requiredLocation) {
            this->requiredLocation = requiredLocation;
        }

        double getTime() const {
            return time;
        }

        void setTime(double time) {
            this->time = time;
        }

        double getTimeToRequiredLocation() const {
            return timeToRequiredLocation;
        }

        void setTimeToRequiredLocation(double timeToRequiredLocation) {
            this->timeToRequiredLocation = timeToRequiredLocation;
        }

        double getWalkDistance() const {
            return walkDistance;
        }

        void setWalkDistance(double walkDistance) {
            this->walkDistance = walkDistance;
        }

        double getWalkTime() const {
            return walkTime;
        }

        void setWalkTime(double walkTime) {
            this->walkTime = walkTime;
        }

        int getXCoord() const {
            return x_coord;
        }

        void setXCoord(int coord) {
            x_coord = coord;
        }

        int getYCoord() const {
            return y_coord;
        }

        void setYCoord(int coord) {
            y_coord = coord;
        }

    double getMaxDelayRequired() const {
        return maxDelayRequired;
    }

    void setMaxDelayRequired(double maxDelayRequired) {
        this->maxDelayRequired = maxDelayRequired;
    }
};

#endif /* STOPPOINT_H_ */
