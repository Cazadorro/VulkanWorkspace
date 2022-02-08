#ifndef VOLUMETRIC_GLSL
#define VOLUMETRIC_GLSL
#include "mathconstants.glsl"
float beer_lambert(float absorption, float distance){
    return exp(-absorption * distance);
}
//same amount of light in every direction
float isotropic_phase(){
    return 1.0/(4.0*3.14);
}
#endif // VOLUMETRIC_GLSL