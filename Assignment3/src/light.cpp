#include "include/light.h"

/**
 * Light
 **/

Light::Light(){
	color = Pixel(255,255,255);
}

Light::Light(Pixel color_){
	color = color_;
}