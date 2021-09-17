#ifndef BASICREFTYPES_GLSL
#define BASICREFTYPES_GLSL

layout(buffer_reference, buffer_reference_align=4, scalar) buffer float32_array{
    float32_t data[];
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer float64_array{
    float64_t data[];
};
layout(buffer_reference, buffer_reference_align=1, scalar) buffer uint8_array{
    uint8_t data[];
};
layout(buffer_reference, buffer_reference_align=2, scalar) buffer uint16_array{
    uint16_t data[];
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer uint32_array{
    uint32_t data[];
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer uint64_array{
    uint64_t data[];
};
layout(buffer_reference, buffer_reference_align=1, scalar) buffer int8_array{
    int8_t data[];
};
layout(buffer_reference, buffer_reference_align=2, scalar) buffer int16_array{
    int16_t data[];
};
layout(buffer_reference, buffer_reference_align=4, scalar) buffer int32_array{
    int32_t data[];
};
layout(buffer_reference, buffer_reference_align=8, scalar) buffer int64_array{
    int64_t data[];
};


#endif //BASICREFTYPES_GLSL