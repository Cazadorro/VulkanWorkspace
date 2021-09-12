#ifndef BASICREFTYPES_GLSL
#define BASICREFTYPES_GLSL

layout(buffer_reference, buffer_reference_align=4, scalar) buffer float32_array{
    float32_t data[];
};


#endif //BASICREFTYPES_GLSL