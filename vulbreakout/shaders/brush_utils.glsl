#ifndef BRUSH_UTILS
#define BRUSH_UTILS
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable

struct BrushShape{
    uint value;
};
//poor mans enum class.
const BrushShape BRUSH_SHAPE_CIRCLE = BrushShape(0);
const BrushShape BRUSH_SHAPE_SQUARE = BrushShape(1);

struct BrushFadeType{
    uint value;
};
//poor mans enum class.
const BrushFadeType BRUSH_FADE_LINEAR = BrushFadeType(0);
const BrushFadeType BRUSH_FADE_CUSTOM_CURVE = BrushFadeType(1);
const BrushFadeType BRUSH_FADE_GUASSIAN = BrushFadeType(2);

//maybe only counts as one "brush engine" though?
//could just have seperate shaders for each though.

//TODO assume we can reinterpret this to be a pointer of uint at the front to get size.
layout(buffer_reference, buffer_reference_align= 4, scalar) buffer Polynomial_ref {
    float data;
};
layout(buffer_reference, buffer_reference_align= 8, scalar) buffer PolynomialList_ref {
    Polynomial_ref data;
};

struct BrushSettingCurveControl{
    uint enabled_settings; //16 values controllable, so 4 values for each setting.
    PolynomialList_ref polynomials;
};

struct BrushSettings{
    BrushShape shape;
    BrushFadeType fade_type;
    vec2 fade_parameter; // interpret this as either vec2 or a pointer to the brush line data depending on fade_type.
//16 bytes at this point.

    float diameter;
    float angle_rad;
    uint spikes; // todo could compress spikes into shape and fadetype, saving space?
    float spacing;
//32 bytes at this point.


    float density;
    uint anti_aliasing_samples; // use multiple samples to get smoother rendered result?
// 40 bytes at this point.
    // add control from pen settings for:
    // opacity
    // flow
    // size
    // ratio
    // spacing
    // mirror (spikes?)
    // softness
    // rotation
    // sharpness? (aa?)
    // lightness strength?
    // scatter? (density)?

    //then color, texture, and masked brush equivalent parameters?

    //add blending mode, a different faked enum class for each value, *then*
    //add blend mode interpreter,  Source, Destination, Never mind, probably don't need, can just directly use GLSL

};

struct TileBbox{
    uint left_x;
    uint top_y;
    uint right_x;
    uint bot_y;
};

uint ceil(uint numerator, uint denominator){
    const uint one = uint(1u);
    return one + ((numerator - one) / denominator);
}

uvec2 ceil(uvec2 numerator, uint denominator){
    return uvec2(ceil(numerator.x, denominator), ceil(numerator.y, denominator));
}

uvec2 ceil(uvec2 numerator, uvec2 denominator){
    return uvec2(ceil(numerator.x, denominator.x), ceil(numerator.y, denominator.y));
}


TileBbox create_bbox(const in BrushSettings brush, vec2 pos, uint tile_dim_px, uvec2 image_size){
    //TODO assuming image size can fit into float, implicitly defining size to be within 2^23.
    float radius = diameter / 2.0;

    TileBbox bbox;
    float left_x = clamp(pos.x - radius, 0.0, float(image_size.x));
    float right_x = clamp(pos.x + radius, 0.0, float(image_size.x));
    float top_y = clamp(pos.y - radius, 0.0, float(image_size.y));
    float bot_y = clamp(pos.y + radius, 0.0, float(image_size.y));


    //TODO don't think I need ceil here, since
//    bbox.left_x = uint(left_x / tile_dim_px);     //floors
//    bbox.right_x = uint(ceil(right_x, tile_dim_px));
//    bbox.top_y = uint(top_y /  tile_dim_px);
//    bbox.bot_y = uint(ceil(bot_y, tile_dim_px));


    bbox.left_x = uint(left_x / tile_dim_px);     //floors
    bbox.right_x = uint(right_x / tile_dim_px);
    bbox.top_y = uint(top_y /  tile_dim_px);
    bbox.bot_y = uint(bot_y /  tile_dim_px);
    return bbox;
}


uint calc_width(const in TileBbox bbox){
    return bbox.right_x - bbox.left_x;
}

uint calc_height(const in TileBbox bbox){
    return bbox.bot_y - bbox.top_y;
}

#endif //BRUSH_UTILS
