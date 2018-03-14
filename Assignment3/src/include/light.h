//light.h
//
//by emma

#ifndef LIGHT_INCLUDED
#define LIGHT_INCLUDED

#include "pixel.h"
#include "coordinate.h"
#include "ray.h"

/**
 * Light
 **/

class Light
{
public:
	Pixel color;

	Light();

	Light(Pixel color);

	void SetColor(Pixel new_color) { color=new_color; }

	virtual Ray CalculateRayFromPoint(Coordinate point) = 0;

	virtual float CalculateDistanceFromPoint(Coordinate point) = 0;

	virtual Coordinate CalculateL_d(Ray ray_toward_light, Ray normal, float distance) = 0;

	virtual Coordinate CalculateL_s(Ray reflected_ray_toward_light, Ray ray_toward_viewer, float phong_n, float distance) = 0;
};

#endif