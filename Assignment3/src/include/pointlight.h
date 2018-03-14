//pointlight.h
//subclass of light
//
//by emma

#ifndef POINTLIGHT_INCLUDED
#define POINTLIGHT_INCLUDED

#include "pixel.h"
#include "coordinate.h"
#include "light.h"
#include "ray.h"

/**
 * PointLight
 **/

class PointLight : public virtual Light
{
public:
	Coordinate pos;

	PointLight();

	PointLight(Pixel color, Coordinate position);

	void SetPosition(Coordinate new_pos) { pos=new_pos; }

	Ray CalculateRayFromPoint(Coordinate point);

	float CalculateDistanceFromPoint(Coordinate point);

	Coordinate CalculateL_d(Ray ray_toward_light, Ray normal, float distance);

	Coordinate CalculateL_s(Ray reflected_ray_toward_light, Ray ray_toward_viewer, float phong_n, float distance);
};
#endif