#ifndef ALGORITHM_GLSL
#define ALGORITHM_GLSL

#include "algorithmf16.glsl"
#include "algorithmf32.glsl"
#include "algorithmf64.glsl"
#include "algorithmu8.glsl"
#include "algorithmu16.glsl"
#include "algorithmu32.glsl"
#include "algorithmu64.glsl"
#include "algorithmi8.glsl"
#include "algorithmi16.glsl"
#include "algorithmi32.glsl"
#include "algorithmi64.glsl"

struct Rangef16{
    float16_t min;
    float16_t max;
};
struct Rangef32{
    float32_t min;
    float32_t max;
};
struct Rangef64{
    float64_t min;
    float64_t max;
};
//psuedo template.
Rangef16 Range(in float16_t min, in float16_t max){
    return Rangef16(min, max);
}
Rangef32 Range(in float32_t min, in float32_t max){
    return Rangef32(min, max);
}
Rangef64 Range(in float64_t min, in float64_t max){
    return Rangef64(min, max);
}
float16_t distance(const in Rangef16 range){
    return range.max - range.min;
}
float32_t distance(const in Rangef32 range){
    return range.max - range.min;
}
float64_t distance(const in Rangef64 range){
    return range.max - range.min;
}

float16_t clamp(float16_t value, const in Rangef16 range){
    return clamp(value, range.min, range.max);
}
float32_t clamp(float32_t value, const in Rangef32 range){
    return clamp(value, range.min, range.max);
}
float64_t clamp(float64_t value, const in Rangef64 range){
    return clamp(value, range.min, range.max);
}


float16_t remap(float16_t value, const in Rangef16 input_range, const in Rangef16 output_range){
    return output_range.min + ( ((value - input_range.min) / (distance(input_range))) * (distance(output_range)) );
}
float32_t remap(float32_t value, const in Rangef32 input_range, const in Rangef32 output_range){
    return output_range.min + ( ((value - input_range.min) / (distance(input_range))) * (distance(output_range)) );
}
float64_t remap(float64_t value, const in Rangef64 input_range, const in Rangef64 output_range){
    return output_range.min + ( ((value - input_range.min) / (distance(input_range))) * (distance(output_range)) );
}

float16_t remap(float16_t value, const in Rangef16 input_range){
    Rangef16 output_range = Rangef16(float16_t(0.0), float16_t(1.0));
    return remap(value, input_range, output_range);
}
float32_t remap(float32_t value, const in Rangef32 input_range){
    Rangef32 output_range = Rangef32(float32_t(0.0), float32_t(1.0));
    return remap(value, input_range, output_range);
}
float64_t remap(float64_t value, const in Rangef64 input_range){
    Rangef64 output_range = Rangef64(float64_t(0.0), float64_t(1.0));
    return remap(value, input_range, output_range);
}

float16_t clamp_remap(float16_t value, const in Rangef16 input_range, const in Rangef16 output_range){
    return clamp(remap(value, input_range, output_range), output_range);
}
float32_t clamp_remap(float32_t value, const in Rangef32 input_range, const in Rangef32 output_range){
    return clamp(remap(value, input_range, output_range), output_range);
}
float64_t clamp_remap(float64_t value, const in Rangef64 input_range, const in Rangef64 output_range){
    return clamp(remap(value, input_range, output_range), output_range);
}

float16_t clamp_remap(float16_t value, const in Rangef16 input_range){
    Rangef16 output_range = Rangef16(float16_t(0.0), float16_t(1.0));
    return clamp(remap(value, input_range, output_range), output_range);
}
float32_t clamp_remap(float32_t value, const in Rangef32 input_range){
    Rangef32 output_range = Rangef32(float32_t(0.0), float32_t(1.0));
    return clamp(remap(value, input_range, output_range), output_range);
}
float64_t clamp_remap(float64_t value, const in Rangef64 input_range){
    Rangef64 output_range = Rangef64(float64_t(0.0), float64_t(1.0));
    return clamp(remap(value, input_range, output_range), output_range);
}

#endif //ALGORITHM_GLSL