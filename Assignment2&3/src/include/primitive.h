//primitive.h
//
//by emma

#ifndef PRIMITIVE_INCLUDED
#define PRIMITIVE_INCLUDED

#include "material.h"
#include "coordinate.h"
#include "ray.h"

/**
 * Primitive
 **/

class Primitive
{
public:
	Material material;

	Primitive();

	Primitive(Material material);

	void SetMaterial(Material new_material) { material=new_material; }

	virtual void GenerateNormals() = 0;

	virtual bool Contains(Coordinate point) = 0;

	virtual Ray CalculateNormal(Coordinate point, Ray ray) = 0;

	virtual bool IsIntersection(Coordinate point) = 0;

	virtual float CalculateTIntersect(Ray ray) = 0;

	virtual float CalculateSecondIntersect(Ray ray) = 0;
};
#endif