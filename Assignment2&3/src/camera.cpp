#include "include/camera.h"

/**
 * Camera
 **/

/*
	Constructors and Destructors
	*/

Camera::Camera(){
	pos = Coordinate(0,0,0); 
	forward = Ray(Coordinate(0,0,0),0,0,1); 
	up = Ray(Coordinate(0,0,0),0,1,0);
	h_angle = 45; //1/2 height angle of the viewing frustum (in degrees)
}

/*
	Getters
	*/

Coordinate Camera::getPos()
{
	return pos;
}

Ray Camera::getForward()
{
	return forward;
}

Ray Camera::getUp()
{
	return up;
}

float Camera::getHeightAngle()
{
	return h_angle;
}

/*
	Setters
	*/

/*
	Others
	*/