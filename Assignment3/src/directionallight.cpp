#include "include/directionallight.h"

/**
 * DirectionalLight
 **/

DirectionalLight::DirectionalLight() : Light(){
	dir = Ray(Coordinate(),0,0,1);
}

DirectionalLight::DirectionalLight(Pixel color_, Ray dir_) : Light(color_)
{
	dir = dir_;
}

Ray DirectionalLight::CalculateRayFromPoint(Coordinate point)
{
	return Ray(point,-dir.x,-dir.y,-dir.z);
}

float DirectionalLight::CalculateDistanceFromPoint(Coordinate point)
{
	return 20;
}

Coordinate DirectionalLight::CalculateL_d(Ray ray_toward_light, Ray normal, float distance)
{
	return Coordinate(color.r*fmax(0.0,DotProduct(normal,ray_toward_light)),
                      color.g*fmax(0.0,DotProduct(normal,ray_toward_light)),
                      color.b*fmax(0.0,DotProduct(normal,ray_toward_light)));
}

Coordinate DirectionalLight::CalculateL_s(Ray reflected_ray_toward_light, Ray ray_toward_viewer, float phong_n, float distance)
{
	return Coordinate(pow(DotProduct(ray_toward_viewer,reflected_ray_toward_light),phong_n)*color.r,
                      pow(DotProduct(ray_toward_viewer,reflected_ray_toward_light),phong_n)*color.g,
                      pow(DotProduct(ray_toward_viewer,reflected_ray_toward_light),phong_n)*color.b);
}