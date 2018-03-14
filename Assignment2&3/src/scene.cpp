#include "include/scene.h"
#include "include/triangle.h"
#include "include/sphere.h"
#include "include/directionallight.h"
#include "include/pointlight.h"
#include <stdlib.h>
#include <string>

//using namespace std;

/**
 * Scene
 **/

/*
  Constructors and Destructors
  */

Scene::Scene()
{
    width = 640;
    height = 480;
    outFile = "raytraced.bmp";
    max_vertices = 0;
    max_normals = 0;
	  prim_count = 0;
	  light_count = 0;
	  amb_light = Coordinate();
	  bg = Pixel(0,0,0);
  	k_r = .5;
    max_depth = 5;
}

/*
  Setters
  */

void Scene::setWidth(int w)
{
  width = w;
}

void Scene::setHeight(int h)
{
  height = h;
}

void Scene::setMaxDepth(int d)
{
  max_depth = d;
}

/*
  Getters
  */

int Scene::getWidth()
{
  return width;
}

int Scene::getHeight()
{
  return height;
}

char Scene::getOutFile()
{
  //return outFile;
}

int Scene::getMaxDepth()
{
  return max_depth;
}



/*
  Others
  */

Pixel EvaluateRayTree(Scene * scene, Camera * cam, Ray ray, int max_depth)
{
	bool hit = false;
  ExtendedRay intersection = ExtendedRay(100000, Coordinate(0,0,0));  //just a default with an
                                                                          //outrageously large t
  float t;
  int prim_index;

  for (int i = 0; i < scene->prim_count; i++){
    t = scene->primitive_list[i]->CalculateTIntersect(ray);
    if ((t > 0) && (t < intersection.t)){
      Coordinate test_point = Evaluate(ray, t);
      if (scene->primitive_list[i]->IsIntersection(test_point)){
        hit = true;
        prim_index = i;
        intersection.SetValues(t, test_point);
      }
    }
  }
        
  if (hit && (max_depth > 0)){
    return ApplyLightingModel(scene, cam, ray, intersection, prim_index, max_depth);
  } else {
    return scene->bg;
  }
}

Pixel ApplyLightingModel(Scene * scene, Camera * cam, Ray ray, ExtendedRay intersection, int prim_index, int max_depth){
	
  Pixel color = Pixel(0,0,0);
  Primitive *hit_primitive = scene->primitive_list[prim_index];

  //calculate normal to intersection point
  Ray n = hit_primitive->CalculateNormal(intersection.p,ray);
  n.Normalize();
        
  for (int j = 0; j < scene->light_count; j++){
    Light *light = scene->light_list[j];

    //for each light, look at the ray going from the intersection point to the light
    Ray shad_ray = light->CalculateRayFromPoint(intersection.p);
    float hit_to_light_dst = light->CalculateDistanceFromPoint(intersection.p);
    shad_ray.Normalize();

    //see if there is anything between the intersection point and the light
    bool hit = false;
    int k = 0;

    while (!hit && k<scene->prim_count){
      float t = scene->primitive_list[k]->CalculateTIntersect(shad_ray);
      Coordinate test_point = Evaluate(shad_ray,t);
      if (scene->primitive_list[k]->IsIntersection(test_point) && (t>.1) && (t<hit_to_light_dst)){
        hit = true;
      }
      k++;
    }

    if (hit){
      //if there is, do nothing
      continue;
            
    } else {
      //if not, calculate and add diffuse/specular colors:
            
      // note: l = shad_ray = ray from intersection toward light (already calculated)
      Coordinate L_d = light->CalculateL_d(shad_ray, n, hit_to_light_dst/*, scene.k_d*/);

      //2. calculate specular lighting!!!
      //already have l (shad_ray) and n

      //r = ray from intersection toward light: reflected version
      Ray r = Ray(n.origin,(2*n.x)-shad_ray.x,(2*n.y)-shad_ray.y,(2*n.z)-shad_ray.z);
      r.Normalize();

      //v = ray from intersection toward viewer/camera
      Ray v = Ray(intersection.p,cam->getPos().getX()-intersection.p.x,cam->getPos().getY()-intersection.p.y,cam->getPos().getZ()-intersection.p.z);
      v.Normalize();

      Coordinate L_s = light->CalculateL_s(v, r, hit_primitive->material.phong_n, hit_to_light_dst/*, scene.k_s*/);

      color = color + Pixel(ComponentClamp(hit_primitive->material.diffuse.r*L_d.x),ComponentClamp(hit_primitive->material.diffuse.g*L_d.y),ComponentClamp(hit_primitive->material.diffuse.b*L_d.z))
                    + Pixel(ComponentClamp(hit_primitive->material.specular.r*L_s.x),ComponentClamp(hit_primitive->material.specular.g*L_s.y),ComponentClamp(hit_primitive->material.specular.b*L_s.z));
          
      //*********************************
                    
      //refraction ray creation and recursion! Sending this ray creation through it's own function in the ray class was buggy so the mess of code is just here instead
      //currently not working so commented out; uncommented version still compiles, but doesn't correctly compute refraction.
      
      // Ray glass;
      // if (hit_primitive->Contains(Evaluate(n,-.001))){ //going through sphere
      //   float n_r = hit_primitive->material.index_of_refraction;
      //   float theta_i = acos(DotProduct(shad_ray,n));
      //   float theta_r = asin(sin(theta_i)/n_r);
      //   Ray T = Ray(n.origin,(cos(theta_i)/n_r-cos(theta_r))*n.x-shad_ray.x/n_r,(cos(theta_i)/n_r-cos(theta_r))*n.y-shad_ray.y/n_r,(cos(theta_i)/n_r-cos(theta_r))*n.z-shad_ray.z/n_r);
      //   glass = Ray(Evaluate(T,hit_primitive->CalculateSecondIntersect(T)),-shad_ray.x,-shad_ray.y,-shad_ray.z);
      // } else { // going through thin surface, ignore effect
      //   glass = Ray(shad_ray.origin,-shad_ray.x,-shad_ray.y,-shad_ray.z);
      // }
      // glass.Normalize();
      // Pixel refraction_color=EvaluateRayTree(scene, cam, glass, max_depth-1);
      // color = color + Pixel(ComponentClamp(scene.k_r*refraction_color.r),ComponentClamp(scene.k_r*refraction_color.g),ComponentClamp(scene.k_r*refraction_color.b));
      
      //*********************************

    }
  }

  //reflection ray creation & recursion!
  Ray mirror = Reflect(ray, n);
  mirror.Normalize();
  Pixel reflection_color=EvaluateRayTree(scene, cam, mirror, max_depth-1);
  color = color + Pixel(ComponentClamp(scene->k_r*reflection_color.r),ComponentClamp(scene->k_r*reflection_color.g),ComponentClamp(scene->k_r*reflection_color.b));

  color = color + Pixel(ComponentClamp(hit_primitive->material.ambient.r*scene->amb_light.x), ComponentClamp(hit_primitive->material.ambient.g*scene->amb_light.y), ComponentClamp(hit_primitive->material.ambient.b*scene->amb_light.z));
  color.SetClamp(color.r, color.g, color.b);
  return color;
}

bool Scene::parseSceneFile(string fileName, Camera * cam)
{
  FILE *fp;
  char line[1024];

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
  //background color (variables for reading from scenefile)
  float bg_r = 0;
  float bg_g = 0;
  float bg_b = 0;
  //point light (variables for reading from scenefile)
  float light_r;
  float light_g;
  float light_b;
  float light_x;
  float light_y;
  float light_z;
  //camera stuff (reading from scenefile)
  float camposx;
  float camposy;
  float camposz;
  float camforx;
  float camfory;
  float camforz;
  float camupx;
  float camupy;
  float camupz;
  float camhangle;

  // open the file containing the scene description
  fp = fopen(fileName.c_str(), "r");

  // check for errors in opening the file
  if (fp == NULL) {
    printf("Can't open file '%s'\n", fileName.c_str());
    return false;  //Exit
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
      sscanf(line,"camera %f %f %f %f %f %f %f %f %f %f",&camposx,&camposy,&camposz,
            &camforx,&camfory,&camforz,&camupx,&camupy,&camupz,&camhangle);
      cam->setPosition(camposx,camposy,camposz);
      cam->setForward(camforx,camfory,camforz);
      cam->setUp(camupx,camupy,camupz);
      cam->setHeightAngle(camhangle);
      printf("Camera position, direction, up vector and angle set.\n");
    }
    
    else if (commandStr == "film_resolution"){ //If the command is a film_resolution command
      sscanf(line,"film_resolution %i %i",&width,&height);
      printf("Film resolution set to %i x %i.\n",width,height);
    }

    else if (commandStr == "output_image"){ //If the command is an output_image command
      char* outFile = new char[1024];
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
      primitive_list.push_back(new Triangle(*vertex_pool[trianglev1],*vertex_pool[trianglev2],*vertex_pool[trianglev3],cur_material));
      primitive_list[prim_count]->GenerateNormals();
      prim_count++;
      printf("Triangle with vertices %i, %i, and %i and current material added to scene.\n",trianglev1,trianglev2,trianglev3);
    }

    else if (commandStr == "normal_triangle"){ //If the command is a normal_triangle command
      printf("Normal triangle implementation currently incomplete: disregarding normal triangle.\n");
    }

    else if (commandStr == "sphere"){ //If the command is a sphere command
      sscanf(line,"sphere %f %f %f %f", &sph_pos_x, &sph_pos_y, &sph_pos_z, &sph_r);
      primitive_list.push_back(new Sphere(Coordinate(sph_pos_x,sph_pos_y,sph_pos_z),sph_r,cur_material));
      prim_count++;
      printf("Sphere at position (%f,%f,%f) with radius %f and current material.\n",sph_pos_x,sph_pos_y,sph_pos_z,sph_r);
    }

    else if (commandStr == "background"){ //If the command is a background command
      sscanf(line,"background %f %f %f", &bg_r, &bg_g, &bg_b);
      bg = Pixel(bg_r*255,bg_g*255,bg_b*255);
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
      light_list.push_back(new DirectionalLight(Pixel(light_r,light_g,light_b),Ray(Coordinate(0,0,0),light_x,light_y,light_z)));
      light_count++;
      printf("New directional light at (%f, %f, %f).\n",light_x,light_y,light_z);
    }

    else if (commandStr == "point_light"){ //If the command is a point_light command
      sscanf(line,"point_light %f %f %f %f %f %f",&light_r,&light_g,&light_b,&light_x,&light_y,&light_z);
      light_list.push_back(new PointLight(Pixel(light_r,light_g,light_b),Coordinate(light_x,light_y,light_z)));
      light_count++;
      printf("New point light at (%f, %f, %f).\n",light_x,light_y,light_z);
    }

    else if (commandStr == "spot_light"){ //If the command is a spot_light command
      printf("Spotlight implementation currently incomplete: disregarding spotlight.\n");
    }

    else if (commandStr == "ambient_light"){ //If the command is an ambient_light command
      sscanf(line,"ambient_light %f %f %f", &amb_light.x, &amb_light.y, &amb_light.z);
      printf("Ambient light multiplier set to rgb (%f, %f, %f).\n",amb_light.x,amb_light.y,amb_light.z);
    }

    else if (commandStr == "max_depth"){ //If the command is an ambient_light command
      sscanf(line,"max_depth %i", &max_depth);
      printf("Maximum recursion depth set to %i.\n",max_depth);
    }
      
    else { //If the command is not recognized
      printf("WARNING. Unknown command: %s.\n",command);
    }
  }

  return true;
}







