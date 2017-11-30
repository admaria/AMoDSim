#ifndef GEOHASH_H_
#define GEOHASH_H_
#include <list>

    typedef enum {
        GEOHASH_NORTH = 0,
        GEOHASH_EAST,
        GEOHASH_WEST,
        GEOHASH_SOUTH
    } GEOHASH_direction;

    typedef struct {
        double max;
        double min;
    } GEOHASH_range;

    typedef struct {
        GEOHASH_range latitude;
        GEOHASH_range longitude;
    } GEOHASH_area;

    typedef struct {
        char* north;
        char* east;
        char* west;
        char* south;
        char* north_east;
        char* south_east;
        char* north_west;
        char* south_west;
    } GEOHASH_neighbors;

class Geohash {
public:
    Geohash();
    virtual bool GEOHASH_verify_hash(const char *hash);
    virtual char* GEOHASH_encode(double latitude, double longitude, unsigned int hash_length);
    virtual GEOHASH_area* GEOHASH_decode(const char* hash);
    virtual GEOHASH_neighbors* GEOHASH_get_neighbors(const char *hash);
    virtual void GEOHASH_free_neighbors(GEOHASH_neighbors *neighbors);
    virtual char* GEOHASH_get_adjacent(const char* hash, GEOHASH_direction dir);
    virtual void GEOHASH_free_area(GEOHASH_area *area);
    virtual std::list<char *> getNeighbors(const char * hash);
};


#endif /* GEOHASH_H_ */
