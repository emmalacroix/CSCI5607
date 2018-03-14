//coordinate.h
//
//Class representing a ray
//by Emma Lacroix

#ifndef COORDINATE_INCLUDED
#define COORDINATE_INCLUDED

/**
 * Coordinate
 **/

class Coordinate
{
public:
    
    float x;
    float y;
    float z;

	// default
	Coordinate ();

    // Creates a coordinate (x, y, z)
    Coordinate (float x, float y, float z);

    // Origin/direction access
    float getX () { return x; }
    float getY () { return y; }
    float getZ () { return z; }

    void setCoord (float new_x, float new_y, float new_z) { x=new_x; y=new_y; z=new_z; }
};

//component-wise addition of two coordinates
Coordinate operator+ (const Coordinate& a, const Coordinate& b);

//component-wise multiplication by a constant
Coordinate operator* (const Coordinate& a, const double b);
#endif