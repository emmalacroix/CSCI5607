//cuboid.h
//
//by Emma Lacroix

#include <vector>
using namespace std;

#ifndef CUBOID_INCLUDED
#define CUBOID_INCLUDED

/**
 * Cuboid
 **/

class Cuboid
{
public:
	float center_x;

	float center_z;

	float base;

	float height; //height

	float radius; //radius

	vector<float> data;

	Cuboid();

	Cuboid(float center_x_, float center_z_, float base_, float height_, float radius_);

	bool Contains(float x, float z);
};
#endif