#include "include/material.h"
#include <stdlib.h>

/**
 * Material
 **/

Material::Material(){
	ambient = Pixel(0,0,0);
	diffuse = Pixel(255,255,255);
	specular = Pixel(0,0,0);
	phong_n = 5;
	transmissive = Pixel(0,0,0);
	index_of_refraction = 1;
}