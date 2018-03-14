//sphere.h
//
//Class representing a sphere!
//by Emma Lacroix


#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include "coordinate.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"

/**
 * Sphere
 **/

class Sphere : public virtual Primitive
{
public:
    
    Coordinate center;
    float radius;

	// default
	Sphere ();

    // Creates a sphere with a given center and radius
    Sphere (Coordinate center, float radius);

    //Creates a sphere with a given center, radius, and material properties
    Sphere (Coordinate center, float radius, Material material);

    // Origin/direction access
    Coordinate GetCenter () { return center; }
    float GetRadius () { return radius; }

    void SetCenter (float new_x, float new_y, float new_z) { center.x=new_x; center.y=new_y; center.z=new_z; }
    void SetRadius (float new_radius) { radius=new_radius; }

    void GenerateNormals();
    bool Contains(Coordinate point);
    bool IsIntersection(Coordinate point);
    float CalculateTIntersect(Ray ray);
    float CalculateSecondIntersect(Ray ray);
    Ray CalculateNormal(Coordinate point, Ray ray);
};
#endif