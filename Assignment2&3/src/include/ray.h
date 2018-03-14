//ray.h
//
//Class representing a ray
//by Emma Lacroix


#ifndef RAY_INCLUDED
#define RAY_INCLUDED

#include "coordinate.h"
#include "math.h"

/**
 * Ray
 **/

class Ray
{
public:
    
    Coordinate origin;
    float x;
    float y;
    float z;

public:
	// default constructor
    Ray ();

    // Creates a ray starting at origin going in direction
    Ray (Coordinate origin, float x, float y, float z);

    // Origin/direction access
    Coordinate GetOrigin () { return origin; }
    float getXDir () { return x; }
    float getYDir () { return y; }
    float getZDir () { return z; }

    void SetOrigin (Coordinate new_origin) { origin = new_origin; }
    void SetDirection (float new_x, float new_y, float new_z) { x = new_x; y = new_y; z = new_z; }

    void Normalize () {
		float length = sqrt(x*x + y*y + z*z);
		this->SetDirection(x/length, y/length, z/length);
	}
};

Coordinate Evaluate (Ray ray, float t);

float Magnitude (Ray ray);

float DotProduct (Ray u, Ray v);

Ray CrossProduct (Ray u, Ray v);

Ray Reflect (Ray ray, Ray normal);

#endif