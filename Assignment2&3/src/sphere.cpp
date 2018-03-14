#include "include/sphere.h"
#include <stdlib.h>
#include <math.h>

/**
 * Sphere
 **/

Sphere::Sphere() : Primitive(){
	center=Coordinate();
	radius=1;
}

Sphere::Sphere (Coordinate center_, float radius_) : Primitive(){
	center=center_;
	radius=radius_;
}

Sphere::Sphere (Coordinate center_, float radius_, Material material_) : Primitive(material_){
	center=center_;
	radius=radius_;
}

void Sphere::GenerateNormals()
{
	return;
}

bool Sphere::Contains (Coordinate point)
{
	if (((point.x-center.x)*(point.x-center.x))+((point.y-center.y)*(point.y-center.y))+
		((point.z-center.z)*(point.z-center.z)) <= (radius*radius)){
		return true;
	} else {
		return false;
	}
}

bool Sphere::IsIntersection (Coordinate point)
{
	if ((((point.x-center.x)*(point.x-center.x))+((point.y-center.y)*(point.y-center.y))+
		((point.z-center.z)*(point.z-center.z)) <= (radius*radius)+.001) || (((point.x-center.x)*(point.x-center.x))+((point.y-center.y)*(point.y-center.y))+
		((point.z-center.z)*(point.z-center.z)) >= (radius*radius)-.001)){
		return true;
	} else {
		return false;
	}
}

Ray Sphere::CalculateNormal(Coordinate point, Ray ray)
{
	return Ray(point,point.x-center.x,point.y-center.y,point.z-center.z);
}

float Sphere::CalculateTIntersect(Ray ray)
{
	Ray L = Ray(ray.origin,center.x-ray.origin.x,center.y-ray.origin.y,center.z-ray.origin.z);
	float t_ca = DotProduct(L,ray);
	float L_mag = Magnitude(L);
	float d = sqrt(L_mag*L_mag - (t_ca*t_ca));
	float t_hc = sqrt(radius*radius - (d*d));
	return t_ca - t_hc;
}

float Sphere::CalculateSecondIntersect(Ray ray)
{
	Ray L = Ray(ray.origin,center.x-ray.origin.x,center.y-ray.origin.y,center.z-ray.origin.z);
	float t_ca = DotProduct(L,ray);
	float L_mag = Magnitude(L);
	float d = sqrt(L_mag*L_mag - (t_ca*t_ca));
	float t_hc = sqrt(radius*radius - (d*d));
	return t_ca + t_hc;
}