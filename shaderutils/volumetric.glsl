#ifndef VOLUMETRIC_GLSL
#define VOLUMETRIC_GLSL
#include "mathconstants.glsl"
#include "mathutils.glsl"
float beer_lambert(float absorption, float distance){
    return exp(-absorption * distance);
}


float powder_law(float absorption, float distance){
    return (1. - exp(-2.*(absorption * distance)));

}
//same amount of light in every direction
float isotropic_phase(){
    return 1.0/(4.0*3.14);
}
//from Real-Time Volumetric Rendering by Patapom / Bomb!
//two circles make the phase function front and back.
float rayleigh_phase(float cos_theta){
    //coeff = 3.0/(16*pi)
    float coeff = 0.0596831036594607509133314112646928857629223671526711682803;
    return coeff * (1.0 + cos_theta*cos_theta);
}
//henyey greenstein phase function.
//anisotropic according to parameters;
//high ecentricity = longer lobe, stretched out circle, ellipse, hence the name eccentricity, more anisotropic, ie biased in a direction
//only single direction phase
float hg_phase(float cos_theta, float eccentricity){
    //coeff = 1.0/(4pi);
    float coeff = 0.07957747154594767;
    float g = eccentricity;
    float g2 = g*g;
    float numerator = 1.0 - g2;
    //pow is an expensive operation, we could simplify via multiplication instead
    //    float denominator = pow((1 + g2 - 2*g*cos_theta), 1.5);
    //might be able to factor this some how to avoid sqrt?
    float temp = (1 + g2 - 2*g*cos_theta);
    float denominator = temp * sqrt(temp);// x^3/2 = x^1.5 = sqrt(x)^3 = x *sqrt(x)
    return (numerator/denominator) * coeff;
}
//an approximation of hg phase function that avoids the sqrt.
float shlick_phase(float cos_theta, float eccentricity){
    //coeff = 1.0/(4pi);
    float coeff = 0.07957747154594767;
    float k = eccentricity;
    float k2 = k*k;
    float numerator = 1.0 - k2;
    float denominator = pow2(1 + k*cos_theta);
    return (numerator / denominator) * coeff;
}
#endif // VOLUMETRIC_GLSL