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
	FILE *fp;
	//long length;
	char line[1024];

	if (argc != 2){
		cout << "Usage: ./raytracer scenefile\n";
    exit(0);
	}

	string fileName = argv[1];

  // defaults!!
  Camera cam = Camera();
  Scene scene = Scene(); //contains list of primitives, list of lights, ambient lighting
                         //and background color info, and diffuse/specular lighting constants
  
  // film resolution
  int width = 640;
  int height = 480;
  //output image file name
  char outFile[] = "raytraced.bmp";
  //background color (variables for reading from scenefile)
  float bg_r = 0;
  float bg_g = 0;
  float bg_b = 0;
  //sphere (variables for reading from scenefile)
  float sph_pos_x;
  float sph_pos_y;
  float sph_pos_z;
  float sph_r;
  //material 
  Material cur_material = Material();
  //(variables for reading from scenefile)
  float amb_m_r = 0;
  float amb_m_g = 0;
  float amb_m_b = 0;
  Pixel amb_rgb;
  float diff_r = 1;
  float diff_g = 1;
  float diff_b = 1;
  Pixel diff_rgb;
  float spec_r = 0;
  float spec_g = 0;
  float spec_b = 0;
  Pixel spec_rgb;
  float phong_cos_pow = 5;
  float trans_r = 0;
  float trans_g = 0;
  float trans_b = 0;
  float refraction_idx = 1;
  //point light (variables for reading from scenefile)
  float light_r;
  float light_g;
  float light_b;
  float light_x;
  float light_y;
  float light_z;
  //max depth
  int max_depth = 5;
  //max vertices/normals, vertex/normal pools, and variables for reading from scenefile
  int max_vertices = 0;
  float vertex1, vertex2, vertex3;
  int vertex_count = 0;
  vector<Coordinate*> vertex_pool;
  int max_normals = 0;
  float normal1, normal2, normal3;
  int normal_count = 0;
  vector<Coordinate*> normal_pool;
  int trianglev1, trianglev2, trianglev3;

	// open the file containing the scene description
	fp = fopen(fileName.c_str(), "r");

	// check for errors in opening the file
  if (fp == NULL) {
    printf("Can't open file '%s'\n", fileName.c_str());
    return 0;  //Exit
  }

  //Loop through reading each line
 	while( fgets(line,1024,fp) ) { //Assumes no line is longer than 1024 characters!
    if (line[0] == '#'){
      printf("Skipping comment: %s", line);
      continue;
    }
    
    char command[100];
    int fieldsRead = sscanf(line,"%s ",command); //Read first word in the line (i.e., the command type)
    string commandStr = command;
    
    if (fieldsRead < 1){ //No command read
    	//Blank line
    	continue;
    }
    
    if (commandStr == "camera"){ //If the command is a camera command
      sscanf(line,"camera %f %f %f %f %f %f %f %f %f %f",&cam.pos.x,&cam.pos.y,&cam.pos.z,
        		&cam.forward.x,&cam.forward.y,&cam.forward.z,&cam.up.x,&cam.up.y,&cam.up.z,&cam.h_angle);
      printf("Camera position, direction, up vector and angle set.\n");
    }
    
    else if (commandStr == "film_resolution"){ //If the command is a film_resolution command
      sscanf(line,"film_resolution %i %i",&width,&height);
      printf("Film resolution set to %i x %i.\n",width,height);
    }

    else if (commandStr == "output_image"){ //If the command is an output_image command
      char outFile[1024];
      sscanf(line,"output_image %s", outFile);
      printf("Render to file named: %s.\n", outFile);
    }

    else if (commandStr == "max_vertices"){ //If the command is a max_vertices command
      sscanf(line,"max_vertices %i",&max_vertices);
      printf("Maximum number of vertices set to %i.\n", max_vertices);
    }

    else if (commandStr == "max_normals"){ //If the command is a max_normals command
      sscanf(line,"max_normals %i",&max_normals);
      printf("Maximum number of vertices set to %i.\n", max_normals);
    }

    else if (commandStr == "vertex"){ //If the command is a vertex command
      sscanf(line,"vertex %f %f %f",&vertex1,&vertex2,&vertex3);
      if (vertex_count < max_vertices){
        vertex_pool.push_back(new Coordinate(vertex1,vertex2,vertex3));
        vertex_count++;
        printf("Vertex %i: (%f, %f, %f) added to vertex pool.\n",vertex_count,vertex1,vertex2,vertex3);
      }
    }

    else if (commandStr == "normal"){ //If the command is a normal command
      sscanf(line,"normal %f %f %f",&normal1,&normal2,&normal3);
      if (normal_count < max_normals){
        normal_pool.push_back(new Coordinate(normal1,normal2,normal3));
        normal_count++;
        printf("Normal (%f, %f, %f) added to normal pool.\n",normal1,normal2,normal3);
      }
    }

    else if (commandStr == "triangle"){ //If the command is a triangle command
      sscanf(line,"triangle %i %i %i",&trianglev1,&trianglev2,&trianglev3);
      scene.primitive_list.push_back(new Triangle(*vertex_pool[trianglev1],*vertex_pool[trianglev2],*vertex_pool[trianglev3],cur_material));
      scene.primitive_list[scene.prim_count]->GenerateNormals();
      scene.prim_count++;
      printf("Triangle with vertices %i, %i, and %i and current material added to scene.\n",trianglev1,trianglev2,trianglev3);
    }

    else if (commandStr == "normal_triangle"){ //If the command is a normal_triangle command
      printf("Normal triangle implementation currently incomplete: disregarding normal triangle.\n");
    }

    else if (commandStr == "sphere"){ //If the command is a sphere command
      sscanf(line,"sphere %f %f %f %f", &sph_pos_x, &sph_pos_y, &sph_pos_z, &sph_r);
      scene.primitive_list.push_back(new Sphere(Coordinate(sph_pos_x,sph_pos_y,sph_pos_z),sph_r,cur_material));
      scene.prim_count++;
      printf("Sphere at position (%f,%f,%f) with radius %f and current material.\n",sph_pos_x,sph_pos_y,sph_pos_z,sph_r);
    }

    else if (commandStr == "background"){ //If the command is a background command
      sscanf(line,"background %f %f %f", &bg_r, &bg_g, &bg_b);
      scene.bg = Pixel(bg_r*255,bg_g*255,bg_b*255);
      printf("Background color set to (%f,%f,%f).\n",bg_r,bg_g,bg_b);
    }

    else if (commandStr == "material"){ //If the command is a material command
      sscanf(line,"material %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
          		&amb_m_r, &amb_m_g, &amb_m_b, 
              &diff_r, &diff_g, &diff_b, 
              &spec_r, &spec_g, &spec_b, 
          		&phong_cos_pow, &trans_r, &trans_g, &trans_b, &refraction_idx);
      cur_material.ambient= Pixel(amb_m_r*255, amb_m_g*255, amb_m_b*255);
      cur_material.diffuse= Pixel(diff_r*255, diff_g*255, diff_b*255);
      cur_material.specular= Pixel(spec_r*255, spec_g*255, spec_b*255);
      cur_material.phong_n= phong_cos_pow;
      cur_material.transmissive= Pixel(trans_r*255, trans_g*255, trans_b*255);
      cur_material.index_of_refraction= refraction_idx;
      printf("Current material set to ambient rgb (%f, %f, %f), diffuse rgb (%f, %f, %f), specular rgb (%f, %f, %f), phong cosine power %f, transmissive rgb (%f, %f, %f), and index of refraction %f.\n",
        amb_m_r, amb_m_g, amb_m_b, diff_r, diff_g, diff_b, spec_r, spec_g, spec_b, phong_cos_pow, trans_r, trans_g, trans_b, refraction_idx);
    }

    else if (commandStr == "directional_light"){ //If the command is a directional_light command
      sscanf(line,"directional_light %f %f %f %f %f %f",&light_r,&light_g,&light_b,&light_x,&light_y,&light_z);
      scene.light_list.push_back(new DirectionalLight(Pixel(light_r,light_g,light_b),Ray(Coordinate(0,0,0),light_x,light_y,light_z)));
      scene.light_count++;
      printf("New directional light at (%f, %f, %f).\n",light_x,light_y,light_z);
    }

    else if (commandStr == "point_light"){ //If the command is a point_light command
      sscanf(line,"point_light %f %f %f %f %f %f",&light_r,&light_g,&light_b,&light_x,&light_y,&light_z);
      scene.light_list.push_back(new PointLight(Pixel(light_r,light_g,light_b),Coordinate(light_x,light_y,light_z)));
      scene.light_count++;
      printf("New point light at (%f, %f, %f).\n",light_x,light_y,light_z);
    }

    else if (commandStr == "spot_light"){ //If the command is a spot_light command
      printf("Spotlight implementation currently incomplete: disregarding spotlight.\n");
    }

    else if (commandStr == "ambient_light"){ //If the command is an ambient_light command
      sscanf(line,"ambient_light %f %f %f", &scene.amb_light.x, &scene.amb_light.y, &scene.amb_light.z);
      printf("Ambient light multiplier set to rgb (%f, %f, %f).\n",scene.amb_light.x,scene.amb_light.y,scene.amb_light.z);
    }

    else if (commandStr == "max_depth"){ //If the command is an ambient_light command
      sscanf(line,"max_depth %i", &max_depth);
      printf("Maximum recursion depth set to %i.\n",max_depth);
    }
    	
    else { //If the command is not recognized
      printf("WARNING. Unknown command: %s.\n",command);
    }
  }

  Image *img = new Image(width,height);

  int x,y;

  //creating coordinate system of camera to use in making rays through viewing plane
  cam.forward.Normalize();
  cam.up.Normalize();
  Ray right = CrossProduct(cam.forward, cam.up);
  right.Normalize();
  float h_angle = (cam.h_angle*M_PI)/180;
  float d = height/(2*tan(h_angle));
  float w_angle = atan(width/(2*d));
  Ray towards_component = Ray(cam.pos,d*cam.forward.x,d*cam.forward.y,d*cam.forward.z);

  for (x = 0; x < width; x++){
    for (y = 0; y < height; y++){
      float adj_x = x-(width/2)+(1/2);
      float adj_y = y-(height/2)+(1/2);

      Ray up_component = Ray(cam.pos,adj_y*cam.up.x,adj_y*cam.up.y,adj_y*cam.up.z);
      Ray right_component = Ray(cam.pos,adj_x*right.x,adj_x*right.y,adj_x*right.z);

      Ray ray = Ray(cam.pos,towards_component.x-up_component.x-right_component.x,
                            towards_component.y-up_component.y-right_component.y,
                            towards_component.z-up_component.z-right_component.z);
      ray.Normalize();

      img->SetPixel(x,y,EvaluateRayTree(scene, cam, ray, max_depth)); //EvaluateRayTree in Scene class
    }
  }
  
  img->Write(outFile);
  return 0;

}












