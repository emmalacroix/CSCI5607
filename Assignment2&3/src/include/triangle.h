//triangle.h
//
//Class representing a triangle!
//by Emma Lacroix


#ifndef TRIANGLE_INCLUDED
#define TRIANGLE_INCLUDED

#include "coordinate.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"

/**
 * Triangle
 **/

class Triangle : public virtual Primitive
{
public:
    
    Coordinate v1;
    Coordinate v2;
    Coordinate v3;
    Ray n1;
    Ray n2;
    Ray n3;

    // Creates a triangle with a three given vertices
    Triangle (Coordinate v1, Coordinate v2, Coordinate v3);

    //Creates a triangle with a three given vertices and material properties
    Triangle (Coordinate v1, Coordinate v2, Coordinate v3, Material material);

    void SetVertices (Coordinate new_v1, Coordinate new_v2, Coordinate new_v3) { v1=new_v1; v2=new_v2; v3=new_v3; }
    void GenerateNormals();

    bool Contains(Coordinate point);
    bool IsIntersection(Coordinate point);
    float CalculateTIntersect(Ray ray);
    float CalculateSecondIntersect(Ray ray);
    Ray CalculateNormal(Coordinate point, Ray ray);
};
#endif