//material.h
//class representing a material
//by emma

#ifndef MATERIAL_INCLUDED
#define MATERIAL_INCLUDED

#include "pixel.h"

class Material
{
public:
	Pixel ambient;
	Pixel diffuse;
	Pixel specular;
	float phong_n;
	Pixel transmissive;
	float index_of_refraction;

	// default
	Material ();

    // Creates a material with given properties
    Material (Pixel ambient_rgb, Pixel diffuse_rgb, Pixel specular_rgb, int phong_n);

    //Getters
    Pixel getAmbient(){ return ambient; }
    Pixel getDiffuse(){ return diffuse; }
    Pixel getSpecular(){ return specular; }
    Pixel getTransmissive(){ return transmissive; }
    float getPhongN(){ return phong_n; }
    float getIOR(){ return index_of_refraction; }

    //Setters
    void SetAmbientRGB (Pixel new_ambient_rgb) { ambient=new_ambient_rgb; }
    void SetDiffuseRGB (Pixel new_diffuse_rgb) { diffuse=new_diffuse_rgb; }
    void SetSpecularRGB (Pixel new_specular_rgb) { specular=new_specular_rgb; }
    void SetPhongN (float new_phong_n) { phong_n=new_phong_n; }
};
#endif
