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

#include <vector>
#include <cstring>

using namespace std;

/**
 * Scene
 **/

class Scene
{
private:
	int width;
	int height;
	char* outFile = new char[1024];
	int max_vertices;
	int max_normals;
	int max_depth;

public:
	std::vector<Primitive*> primitive_list;
	int prim_count;
	std::vector<Light*> light_list;
	int light_count;
	Coordinate amb_light;
	Pixel bg;
  	float k_r; /*reflection coefficient*/

	//constructors and destructors
	Scene();

	//setters
	void setWidth(int w);
	void setHeight(int h);
	void setMaxDepth(int d);

	//getters
	int getWidth();
	int getHeight();
	char getOutFile();
	int getMaxDepth();

	//others
	bool parseSceneFile(string fileName, Camera * cam);
};

Pixel EvaluateRayTree(Scene * scene, Camera * cam, Ray ray, int max_depth);

Pixel ApplyLightingModel(Scene * scene, Camera * cam, Ray ray, ExtendedRay intersection, int prim_index, int max_depth);

#endif