#include "include/ray.h"
#include <stdlib.h>
#include <math.h>
#include <cstdio>

/**
 * Ray
 **/

Ray::Ray(){}

Ray::Ray (Coordinate origin_, float x_, float y_, float z_){
	origin = origin_;
	x = x_;
	y = y_;
	z = z_;
}

Coordinate Evaluate (Ray ray, float t) {
	return Coordinate(ray.origin.x+(ray.x*t),ray.origin.y+(ray.y*t),ray.origin.z+(ray.z*t));
}

float Magnitude(Ray ray) {
	return sqrt(ray.x*ray.x + ray.y*ray.y + ray.z*ray.z);
}

float DotProduct (Ray u, Ray v)
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}


Ray CrossProduct (Ray u, Ray v)
{
	//assumed same origin
	return Ray(u.origin,(u.y*v.z)-(u.z*v.y), (u.z*v.x)-(u.x*v.z), (u.x*v.y)-(u.y*v.x));
}

Ray Reflect (Ray ray, Ray normal)
{
	float dotp = -DotProduct(normal, ray);
	return Ray(normal.origin,(2*dotp*normal.x)+ray.x,(2*dotp*normal.y)+ray.y,(2*dotp*normal.z)+ray.z);
}
