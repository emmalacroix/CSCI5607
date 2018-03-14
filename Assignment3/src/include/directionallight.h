//directionallight.h
//subclass of light
//
//by emma

#ifndef DIRECTIONALLIGHT_INCLUDED
#define DIRECTIONALLIGHT_INCLUDED

#include "pixel.h"
#include "coordinate.h"
#include "light.h"
#include "ray.h"

/**
 * DirectionalLight
 **/


class DirectionalLight : public virtual Light
{
public:
	Ray dir;

	DirectionalLight();

	DirectionalLight(Pixel color, Ray direction);

	void SetDirection(Ray new_dir) { dir=new_dir; }

	Ray CalculateRayFromPoint(Coordinate point);

	float CalculateDistanceFromPoint(Coordinate point);

	Coordinate CalculateL_d(Ray ray_toward_light, Ray normal, float distance);

	Coordinate CalculateL_s(Ray reflected_ray_toward_light, Ray ray_toward_viewer, float phong_n, float distance);
};
#endif