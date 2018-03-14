#include "include/primitive.h"
#include <stdlib.h>

/**
 * Primitive
 **/

Primitive::Primitive(){
	material=Material();
}

Primitive::Primitive (Material material_){
	material=material_;
}