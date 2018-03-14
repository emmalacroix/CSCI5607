#include "include/coordinate.h"
#include <stdlib.h>

/**
 * Coordinate
 **/

Coordinate::Coordinate(){
	x=0;
	y=0;
	z=0;
}

Coordinate::Coordinate (float x_, float y_, float z_){
	x = x_;
	y = y_;
	z = z_;
}

Coordinate operator+ (const Coordinate& a, const Coordinate& b)
{
	return Coordinate(a.x+b.x, a.y+b.y, a.z+b.z);
}

Coordinate operator* (const Coordinate& a, double b)
{
    return Coordinate(a.x*b,a.y*b,a.z*b);
}

