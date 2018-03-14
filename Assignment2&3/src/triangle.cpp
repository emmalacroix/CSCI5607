#include "include/triangle.h"
#include <stdlib.h>

/**
 * Triangle
 **/

Triangle::Triangle (Coordinate v1_, Coordinate v2_, Coordinate v3_) : Primitive(){
	v1 = v1_;
	v2 = v2_;
	v3 = v3_;
}

Triangle::Triangle (Coordinate v1_, Coordinate v2_, Coordinate v3_, Material material_) : Primitive(material_){
	v1 = v1_;
	v2 = v2_;
	v3 = v3_;
}

void Triangle::GenerateNormals()
{
	n1 = CrossProduct(Ray(v1,v2.x-v1.x,v2.y-v1.y,v2.z-v1.z),Ray(v1,v3.x-v1.x,v3.y-v1.y,v3.z-v1.z));
	n2 = CrossProduct(Ray(v2,v1.x-v2.x,v1.y-v2.y,v1.z-v2.z),Ray(v2,v3.x-v2.x,v3.y-v2.y,v3.z-v2.z));
	n3 = CrossProduct(Ray(v3,v1.x-v3.x,v1.y-v3.y,v1.z-v3.z),Ray(v3,v2.x-v3.x,v2.y-v3.y,v2.z-v3.z));
	n1.Normalize();
	n2.Normalize();
	n3.Normalize();
}

float Triangle::CalculateTIntersect(Ray ray) //ray is ray from camera
{
	return (DotProduct(Ray(v1,v1.x-ray.origin.x,v1.y-ray.origin.y,v1.z-ray.origin.z),n1)/DotProduct(ray,n1));
}

float Triangle::CalculateSecondIntersect(Ray ray) //this function only really needed for sphere, but pure virtual -> implemented here anyway
{
	return (DotProduct(Ray(v1,v1.x-ray.origin.x,v1.y-ray.origin.y,v1.z-ray.origin.z),n1)/DotProduct(ray,n1));
}

bool Triangle::Contains(Coordinate point) //this function only really needed for sphere, but pure virtual -> implemented here anyway
{
	float area = Magnitude(CrossProduct(Ray(v1,v3.x-v1.x,v3.y-v1.y,v3.z-v1.z),Ray(v1,v2.x-v1.x,v2.y-v1.y,v2.z-v1.z)))/2;
	float u = Magnitude(CrossProduct(Ray(v1,v3.x-v1.x,v3.y-v1.y,v3.z-v1.z),Ray(v1,point.x-v1.x,point.y-v1.y,point.z-v1.z)))/(2*area);
	float v = Magnitude(CrossProduct(Ray(v1,v2.x-v1.x,v2.y-v1.y,v2.z-v1.z),Ray(v1,point.x-v1.x,point.y-v1.y,point.z-v1.z)))/(2*area);
	float w = Magnitude(CrossProduct(Ray(v3,v2.x-v3.x,v2.y-v3.y,v2.z-v3.z),Ray(v3,point.x-v3.x,point.y-v3.y,point.z-v3.z)))/(2*area);
	if (u>=-.001 && u<=1.001 && v>=-.001 && v<=1.001 && w>=-.001 && w<=1.001 && (u+v+w>=.999) && (u+v+w<=1.001)){
		return true;
	} else {
		return false;
	}
}

bool Triangle::IsIntersection(Coordinate point) // ray is ray from camera
{
	float area = Magnitude(CrossProduct(Ray(v1,v3.x-v1.x,v3.y-v1.y,v3.z-v1.z),Ray(v1,v2.x-v1.x,v2.y-v1.y,v2.z-v1.z)))/2;
	float u = Magnitude(CrossProduct(Ray(v1,v3.x-v1.x,v3.y-v1.y,v3.z-v1.z),Ray(v1,point.x-v1.x,point.y-v1.y,point.z-v1.z)))/(2*area);
	float v = Magnitude(CrossProduct(Ray(v1,v2.x-v1.x,v2.y-v1.y,v2.z-v1.z),Ray(v1,point.x-v1.x,point.y-v1.y,point.z-v1.z)))/(2*area);
	float w = Magnitude(CrossProduct(Ray(v3,v2.x-v3.x,v2.y-v3.y,v2.z-v3.z),Ray(v3,point.x-v3.x,point.y-v3.y,point.z-v3.z)))/(2*area);
	if (u>=-.001 && u<=1.001 && v>=-.001 && v<=1.001 && w>=-.001 && w<=1.001 && (u+v+w>=.999) && (u+v+w<=1.001)){
		return true;
	} else {
		return false;
	}
}

Ray Triangle::CalculateNormal(Coordinate point, Ray ray) // ray is ray from camera
{
	if (DotProduct(n1, ray) >= 0){
		return Ray(point,-n1.x,-n1.y,-n1.z);
	} else {
		return Ray(point,n1.x,n1.y,n1.z);
	}
}







