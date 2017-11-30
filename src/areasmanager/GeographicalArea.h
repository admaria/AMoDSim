
#ifndef GEOGRAPHICALAREA_H_
#define GEOGRAPHICALAREA_H_

#include <cobject.h>
#include <list>
#include <string>

class GeographicalArea: public cObject {
private:
    char *hash;
    std::list<std::string> nodes;
    double meanTravelTime;

public:
    GeographicalArea();
    GeographicalArea(char* hash);
    virtual ~GeographicalArea();

    virtual std::list<std::string> getNodes() const;
    virtual void addNode(std::string nodeID);
    virtual void setMeanTravelTime(double meanTime);
    virtual double getMeanTravelTime() const;
};

#endif /* GEOGRAPHICALAREA_H_ */
