#include "include/pointlight.h"

/**
 * PointLight
 **/

PointLight::PointLight() : Light(){
	pos = Coordinate(0,0,0);
}

PointLight::PointLight(Pixel color_, Coordinate pos_) : Light(color_)
{
	pos = pos_;
}

Ray PointLight::CalculateRayFromPoint(Coordinate point)
{
	return Ray(point,pos.x-point.x,pos.y-point.y,pos.z-point.z);
}

float PointLight::CalculateDistanceFromPoint(Coordinate point)
{
	Ray ray = CalculateRayFromPoint(point);
	return sqrt(DotProduct(ray,ray));
}

Coordinate PointLight::CalculateL_d(Ray ray_toward_light, Ray normal, float distance)
{
	return Coordinate((color.r*fmax(0.0,DotProduct(normal,ray_toward_light)))/(distance*distance),
                      (color.g*fmax(0.0,DotProduct(normal,ray_toward_light)))/(distance*distance),
                      (color.b*fmax(0.0,DotProduct(normal,ray_toward_light)))/(distance*distance));
}

Coordinate PointLight::CalculateL_s(Ray reflected_ray_toward_light, Ray ray_toward_viewer, float phong_n, float distance)
{
	return Coordinate((pow(DotProduct(ray_toward_viewer,reflected_ray_toward_light),phong_n)*color.r)/(distance*distance),
                      (pow(DotProduct(ray_toward_viewer,reflected_ray_toward_light),phong_n)*color.g)/(distance*distance),
                      (pow(DotProduct(ray_toward_viewer,reflected_ray_toward_light),phong_n)*color.b)/(distance*distance));
}