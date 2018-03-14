#include "include/scene.h"
#include <stdlib.h>

//using namespace std;

/**
 * Scene
 **/

Scene::Scene(){
	  prim_count = 0;
	  light_count = 0;
	  amb_light = Coordinate();
	  bg = Pixel(0,0,0);
  	k_r = .5;
}

Pixel EvaluateRayTree(Scene scene, Camera cam, Ray ray, int max_depth)
{
	bool hit = false;
  ExtendedRay intersection = ExtendedRay(100000, Coordinate(0,0,0));  //just a default with an
                                                                          //outrageously large t
  float t;
  int prim_index;

  for (int i = 0; i < scene.prim_count; i++){
    t = scene.primitive_list[i]->CalculateTIntersect(ray);
    if ((t > 0) && (t < intersection.t)){
      Coordinate test_point = Evaluate(ray, t);
      if (scene.primitive_list[i]->IsIntersection(test_point)){
        hit = true;
        prim_index = i;
        intersection.SetValues(t, test_point);
      }
    }
  }
        
  if (hit && (max_depth > 0)){
    return ApplyLightingModel(scene, cam, ray, intersection, prim_index, max_depth);
  } else {
    return scene.bg;
  }
}

Pixel ApplyLightingModel(Scene scene, Camera cam, Ray ray, ExtendedRay intersection, int prim_index, int max_depth){
	
  Pixel color = Pixel(0,0,0);
  Primitive *hit_primitive = scene.primitive_list[prim_index];

  //calculate normal to intersection point
  Ray n = hit_primitive->CalculateNormal(intersection.p,ray);
  n.Normalize();
        
  for (int j = 0; j < scene.light_count; j++){
    Light *light = scene.light_list[j];

    //for each light, look at the ray going from the intersection point to the light
    Ray shad_ray = light->CalculateRayFromPoint(intersection.p);
    float hit_to_light_dst = light->CalculateDistanceFromPoint(intersection.p);
    shad_ray.Normalize();

    //see if there is anything between the intersection point and the light
    bool hit = false;
    int k = 0;

    while (!hit && k<scene.prim_count){
      float t = scene.primitive_list[k]->CalculateTIntersect(shad_ray);
      Coordinate test_point = Evaluate(shad_ray,t);
      if (scene.primitive_list[k]->IsIntersection(test_point) && (t>.1) && (t<hit_to_light_dst)){
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
      Ray v = Ray(intersection.p,cam.pos.x-intersection.p.x,cam.pos.y-intersection.p.y,cam.pos.z-intersection.p.z);
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
  color = color + Pixel(ComponentClamp(scene.k_r*reflection_color.r),ComponentClamp(scene.k_r*reflection_color.g),ComponentClamp(scene.k_r*reflection_color.b));

  color = color + Pixel(ComponentClamp(hit_primitive->material.ambient.r*scene.amb_light.x), ComponentClamp(hit_primitive->material.ambient.g*scene.amb_light.y), ComponentClamp(hit_primitive->material.ambient.b*scene.amb_light.z));
  color.SetClamp(color.r, color.g, color.b);
  return color;
}









