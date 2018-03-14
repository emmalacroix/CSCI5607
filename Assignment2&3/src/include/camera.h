//Camera.h
//
//class representing a camera
//by emma

#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED

#include "coordinate.h"
#include "ray.h"

class Camera
{
public:
	Coordinate pos; //position
	Ray forward; //direction 
	Ray up; //up vector
	float h_angle; //1/2 height angle of the viewing frustum

public:
	//default
	Camera ();

	//getters
	Coordinate getPos();
	Ray getForward();
	Ray getUp();
	float getHeightAngle();

	//setters
	void setPosition(float new_x, float new_y, float new_z) { pos=Coordinate(new_x, new_y, new_z); }
	void setForward(float new_x, float new_y, float new_z) { forward=Ray(this->pos, new_x, new_y, new_z); }
	void setUp(float new_x, float new_y, float new_z) { up=Ray(this->pos, new_x, new_y, new_z); }
	void setHeightAngle(float new_angle) { h_angle=new_angle; }

};

#endif