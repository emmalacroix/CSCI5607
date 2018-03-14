//Raytracer
//by Emma Lacroix
//
//Skeleton code taken from:
//
//Image Manipulation Skeleton Code
//  main.c original by Wagner Correa, 1999
//  modified by Robert Osada, 2000
//  modified by Renato Werneck, 2003
//  modified by Stephen J. Guy, 2010-2017
//
//Referenced the file parsing code example using sscanf provided in class
//
//Explanation of ray/circle intersection referenced from:
//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
//
//Explanation of ray/plane (ie. triangle) intersection referenced from:
//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
//
//Explanation of barycentric coordinates referenced from:
//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates

#include "include/coordinate.h"
#include "include/ray.h"
#include "include/extendedray.h"
#include "include/primitive.h"
#include "include/sphere.h"
#include "include/triangle.h"
#include "include/pixel.h"
#include "include/pointlight.h"
#include "include/directionallight.h"
#include "include/camera.h"
#include "include/scene.h"
#include "include/image.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <vector>

#define _USE_MATH_DEFINES /* so we can use pi */

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "include/stb_image_write.h"

using namespace std;

/**
 * Raytracer!
 **/

int main( int argc, char** argv ){

	if (argc != 2){
		cout << "Usage: ./raytracer scenefile\n";
    exit(0);
	}

	string fileName = argv[1];

  // defaults!!
  Camera * cam = new Camera();
  Scene * scene = new Scene(); //contains list of primitives, list of lights, ambient lighting
                         //and background color info, and diffuse/specular lighting constants

  //parse the scenefile
  if (scene->parseSceneFile(fileName, cam) == false)
  {
    printf("Can't open file '%s'\n", fileName.c_str());
    return 0;  //Exit
  }

  int width = scene->getWidth();
  int height = scene->getHeight();

  Image *img = new Image(width,height);

  int x,y;

  //creating coordinate system of camera to use in making rays through viewing plane
  cam->getForward().Normalize();
  cam->getUp().Normalize();
  Ray right = CrossProduct(cam->getForward(), cam->getUp());
  //right.Normalize();
  float h_angle = (cam->getHeightAngle()*M_PI)/180;
  float d = height/(2*tan(h_angle));
  float w_angle = atan(width/(2*d));
  Ray towards_component = Ray(cam->getPos(),d*cam->getForward().getXDir(),d*cam->getForward().getYDir(),d*cam->getForward().getZDir());

  for (x = 0; x < width; x++){
    for (y = 0; y < height; y++){
      float adj_x = x-(width/2)+(1/2);
      float adj_y = y-(height/2)+(1/2);

      Ray up_component = Ray(cam->getPos(),adj_y*cam->getUp().getXDir(),adj_y*cam->getUp().getYDir(),adj_y*cam->getUp().getZDir());
      Ray right_component = Ray(cam->getPos(),adj_x*right.x,adj_x*right.y,adj_x*right.z);

      Ray ray = Ray(cam->getPos(),towards_component.x-up_component.x-right_component.x,
                            towards_component.y-up_component.y-right_component.y,
                            towards_component.z-up_component.z-right_component.z);
      ray.Normalize();

      img->SetPixel(x,y,EvaluateRayTree(scene, cam, ray, scene->getMaxDepth())); //EvaluateRayTree in Scene class
    }
  }
  
  img->Write("raytraced.bmp");
  return 0;

}












