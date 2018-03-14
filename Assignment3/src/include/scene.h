//scene.h
//
//by emma

#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include "primitive.h"
#include "light.h"
#include "coordinate.h"
#include "ray.h"
#include "camera.h"
#include "extendedray.h"
#include "vector"

using namespace std;

/**
 * Scene
 **/

class Scene
{
public:
	std::vector<Primitive*> primitive_list;
	int prim_count;
	std::vector<Light*> light_list;
	int light_count;
	Coordinate amb_light;
	Pixel bg;
  	float k_r; /*reflection coefficient*/

	Scene();
};

Pixel EvaluateRayTree(Scene scene, Camera cam, Ray ray, int max_depth);

Pixel ApplyLightingModel(Scene scene, Camera cam, Ray ray, ExtendedRay intersection, int prim_index, int max_depth);

#endif