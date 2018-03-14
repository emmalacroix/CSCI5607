//extendedray.h
//
//by emma
//holds information about a ray evaluated at a certain t

#ifndef EXTENDEDRAY_INCLUDED
#define EXTENDEDRAY_INCLUDED

#include "coordinate.h"

/**
 * Extended Ray
 **/

class ExtendedRay
{
public:
	float t;

	Coordinate p;

	ExtendedRay(float t, Coordinate p);

	void SetValues(float new_t, Coordinate new_p) { t=new_t; p=new_p; }
};
#endif