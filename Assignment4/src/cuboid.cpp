#include "include/cuboid.h"
#include <vector>
using namespace std;

/**
 * Cuboid
 **/

Cuboid::Cuboid(float center_x_, float center_z_, float base_, float height_, float radius_)
{
	center_x = center_x_;
	center_z = center_z_; //center, on bottom face
	base = base_;
	height = height_;
	radius = radius_;
	float data_info[] = {
		//X, Y, Z, U, V, N_X, N_Y, N_Z
		//face 1
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, 0.0, -1.0,
		center_x-radius, base, center_z-radius, 1.0, 0.0, 0.0, 0.0, -1.0,
		center_x-radius, base+height, center_z-radius, 1.0, 1.0, 0.0, 0.0, -1.0,
		center_x-radius, base+height, center_z-radius, 1.0, 1.0, 0.0, 0.0, -1.0,
		center_x+radius, base+height, center_z-radius, 0.0, 1.0, 0.0, 0.0, -1.0,
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, 0.0, -1.0,
		//face 2
		center_x-radius, base, center_z-radius, 0.0, 0.0, -1.0, 0.0, 0.0,
		center_x-radius, base, center_z+radius, 1.0, 0.0, -1.0, 0.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, -1.0, 0.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, -1.0, 0.0, 0.0,
		center_x-radius, base+height, center_z-radius, 0.0, 1.0, -1.0, 0.0, 0.0,
		center_x-radius, base, center_z-radius, 0.0, 0.0, -1.0, 0.0, 0.0,
		//face 3
		center_x-radius, base, center_z+radius, 0.0, 0.0, 0.0, 0.0, 1.0,
		center_x+radius, base, center_z+radius, 1.0, 0.0, 0.0, 0.0, 1.0,
		center_x+radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 0.0, 1.0,
		center_x+radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 0.0, 1.0,
		center_x-radius, base+height, center_z+radius, 0.0, 1.0, 0.0, 0.0, 1.0,
		center_x-radius, base, center_z+radius, 0.0, 0.0, 0.0, 0.0, 1.0,
		//face 4
		center_x+radius, base, center_z+radius, 0.0, 0.0, 1.0, 0.0, 0.0,
		center_x+radius, base, center_z-radius, 1.0, 0.0, 1.0, 0.0, 0.0,
		center_x+radius, base+height, center_z-radius, 1.0, 1.0, 1.0, 0, 0.0,
		center_x+radius, base+height, center_z-radius, 1.0, 1.0, 1.0, 0, 0.0,
		center_x+radius, base+height, center_z+radius, 0.0, 1.0, 1.0, 0.0, 0.0,
		center_x+radius, base, center_z+radius, 0.0, 0.0, 1.0, 0.0, 0.0,
		//face 5
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, -1.0, 0.0,
		center_x-radius, base, center_z-radius, 1.0, 0.0, 0.0, -1.0, 0.0,
		center_x-radius, base, center_z+radius, 1.0, 1.0, 0.0, -1.0, 0.0,
		center_x-radius, base, center_z+radius, 1.0, 1.0, 0.0, -1.0, 0.0,
		center_x+radius, base, center_z+radius, 0.0, 1.0, 0.0, -1.0, 0.0,
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, -1.0, 0.0,
		//face 6
		center_x+radius, base+height, center_z-radius, 0.0, 0.0, 0.0, 1.0, 0.0,
		center_x-radius, base+height, center_z-radius, 1.0, 0.0, 0.0, 1.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 1.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 1.0, 0.0,
		center_x+radius, base+height, center_z+radius, 0.0, 1.0, 0.0, 1.0, 0.0,
		center_x+radius, base+height, center_z-radius, 0.0, 0.0, 0.0, 1.0, 0.0
	};
	for (int i = 0; i < 288; i++){
		data.push_back(data_info[i]);
	}
}

Cuboid::Cuboid()
{
	center_x = -1;
	center_z = -1; //center, on bottom face
	base = 0;
	height = 0;
	radius = 0;
	float data_info[] = {
		//X, Y, Z, U, V, N_X, N_Y, N_Z
		//face 1
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, 0.0, -1.0,
		center_x-radius, base, center_z-radius, 1.0, 0.0, 0.0, 0.0, -1.0,
		center_x-radius, base+height, center_z-radius, 1.0, 1.0, 0.0, 0.0, -1.0,
		center_x-radius, base+height, center_z-radius, 1.0, 1.0, 0.0, 0.0, -1.0,
		center_x+radius, base+height, center_z-radius, 0.0, 1.0, 0.0, 0.0, -1.0,
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, 0.0, -1.0,
		//face 2
		center_x-radius, base, center_z-radius, 0.0, 0.0, -1.0, 0.0, 0.0,
		center_x-radius, base, center_z+radius, 1.0, 0.0, -1.0, 0.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, -1.0, 0.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, -1.0, 0.0, 0.0,
		center_x-radius, base+height, center_z-radius, 0.0, 1.0, -1.0, 0.0, 0.0,
		center_x-radius, base, center_z-radius, 0.0, 0.0, -1.0, 0.0, 0.0,
		//face 3
		center_x-radius, base, center_z+radius, 0.0, 0.0, 0.0, 0.0, 1.0,
		center_x+radius, base, center_z+radius, 1.0, 0.0, 0.0, 0.0, 1.0,
		center_x+radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 0.0, 1.0,
		center_x+radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 0.0, 1.0,
		center_x-radius, base+height, center_z+radius, 0.0, 1.0, 0.0, 0.0, 1.0,
		center_x-radius, base, center_z+radius, 0.0, 0.0, 0.0, 0.0, 1.0,
		//face 4
		center_x+radius, base, center_z+radius, 0.0, 0.0, 1.0, 0.0, 0.0,
		center_x+radius, base, center_z-radius, 1.0, 0.0, 1.0, 0.0, 0.0,
		center_x+radius, base+height, center_z-radius, 1.0, 1.0, 1.0, 0, 0.0,
		center_x+radius, base+height, center_z-radius, 1.0, 1.0, 1.0, 0, 0.0,
		center_x+radius, base+height, center_z+radius, 0.0, 1.0, 1.0, 0.0, 0.0,
		center_x+radius, base, center_z+radius, 0.0, 0.0, 1.0, 0.0, 0.0,
		//face 5
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, -1.0, 0.0,
		center_x-radius, base, center_z-radius, 1.0, 0.0, 0.0, -1.0, 0.0,
		center_x-radius, base, center_z+radius, 1.0, 1.0, 0.0, -1.0, 0.0,
		center_x-radius, base, center_z+radius, 1.0, 1.0, 0.0, -1.0, 0.0,
		center_x+radius, base, center_z+radius, 0.0, 1.0, 0.0, -1.0, 0.0,
		center_x+radius, base, center_z-radius, 0.0, 0.0, 0.0, -1.0, 0.0,
		//face 6
		center_x+radius, base+height, center_z-radius, 0.0, 0.0, 0.0, 1.0, 0.0,
		center_x-radius, base+height, center_z-radius, 1.0, 0.0, 0.0, 1.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 1.0, 0.0,
		center_x-radius, base+height, center_z+radius, 1.0, 1.0, 0.0, 1.0, 0.0,
		center_x+radius, base+height, center_z+radius, 0.0, 1.0, 0.0, 1.0, 0.0,
		center_x+radius, base+height, center_z-radius, 0.0, 0.0, 0.0, 1.0, 0.0
	};
	for (int i = 0; i < 288; i++){
		data.push_back(data_info[i]);
	}
}

bool Cuboid::Contains(float x, float z){
	if ((x>center_x-radius)&&(x<center_x+radius)&&(z>center_z-radius)&&(z<center_z+radius)){
		return true;
	} else {
		return false;
	}
}











