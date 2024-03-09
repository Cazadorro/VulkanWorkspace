#ifndef BLEND_UTILS
#define BLEND_UTILS
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable
#include "algorithmf32.glsl"
////factor for alpha, factor for color
////blend op for color, blend op for alpha.
////
//struct BlendOp{
//    uint8_t value;
//};
//
////split blend modes into bits inside uint32 and blend factors?
////what about handling alpha?
////alpha blending + color blending, then how to combine?
////what about pre-multiplying alpha?
//
////combine blend ops with blend factors? instead of "op mul add sum" could just do that?
//const BlendOp BLEND_OP_ADD          = BlendOp(uint16_t( 0u)); // a + b
//const BlendOp BLEND_OP_SUB          = BlendOp(uint16_t( 1u)); // a - b
//const BlendOp BLEND_OP_MUL          = BlendOp(uint16_t( 2u)); // a * b
//const BlendOp BLEND_OP_DIV          = BlendOp(uint16_t( 3u)); // a / b
//const BlendOp BLEND_OP_MUL_ADD_SUM  = BlendOp(uint16_t( 4u)); // a + b + a*b
//const BlendOp BLEND_OP_MUL_SUB_SUM  = BlendOp(uint16_t( 5u)); // a + b - a*b
//const BlendOp BLEND_OP_DIFF         = BlendOp(uint16_t( 6u)); // abs(a - b)
//const BlendOp BLEND_OP_MIN          = BlendOp(uint16_t( 7u)); // min(a,b)
//const BlendOp BLEND_OP_MAX          = BlendOp(uint16_t( 8u)); // max(a,b)
//const BlendOp BLEND_OP_INV_MUL      = BlendOp(uint16_t( 9u)); // 1 - a*b
//const BlendOp BLEND_OP_EXCLUSION    = BlendOp(uint16_t(10u)); // a + b - 2*a*b
//
//// (1 - a) * b = b * a - 1
////independently take the max of one
//
////Second source? Use it to implement screen?
//
////figure out what to do with pre-multiplied alpha
//struct BlendFactor{
//    uint8_t value;
//};
//
//const BlendFactor BLEND_FACTOR_ZERO          = BlendFactor(uint16_t(0u));
//const BlendFactor BLEND_FACTOR_ONE           = BlendFactor(uint16_t(1u));
//const BlendFactor BLEND_FACTOR_SRC           = BlendFactor(uint16_t(2u));
//const BlendFactor BLEND_FACTOR_ONE_MINUS_SRC = BlendFactor(uint16_t(3u));
//const BlendFactor BLEND_FACTOR_DST           = BlendFactor(uint16_t(4u));
//const BlendFactor BLEND_FACTOR_ONE_MINUS_DST = BlendFactor(uint16_t(5u));
//
//
//const BlendOp BLEND_OP_ZERO          = BlendOp(uint16_t( 0u)); // 0
//const BlendOp BLEND_OP_ONE           = BlendOp(uint16_t( 1u)); // 1
//const BlendOp BLEND_OP_SRC           = BlendOp(uint16_t( 2u)); // a
//const BlendOp BLEND_OP_ONE_MINUS_SRC = BlendOp(uint16_t( 3u)); // 1 - a
//const BlendOp BLEND_OP_DST           = BlendOp(uint16_t( 4u)); // a
//const BlendOp BLEND_OP_ONE_MINUS_DST = BlendOp(uint16_t( 5u)); // 1 - b
//const BlendOp BLEND_OP_ADD           = BlendOp(uint16_t( 6u)); // a + b
//const BlendOp BLEND_OP_SUB           = BlendOp(uint16_t( 7u)); // a - b
//const BlendOp BLEND_OP_MUL           = BlendOp(uint16_t( 8u)); // a * b
//const BlendOp BLEND_OP_DIV           = BlendOp(uint16_t( 9u)); // a / b
//const BlendOp BLEND_OP_DIFF          = BlendOp(uint16_t(10u)); // abs(a - b)
//const BlendOp BLEND_OP_MIN           = BlendOp(uint16_t(11u)); // min(a,b)
//const BlendOp BLEND_OP_MAX           = BlendOp(uint16_t(12u)); // max(a,b)
//const BlendOp BLEND_OP_ONE_MINUS_MUL = BlendOp(uint16_t(13u)); // 1 - (a * b)
//const BlendOp BLEND_OP_ONE_MINUS_DIV = BlendOp(uint16_t(13u)); // 1 - (a / b)
////constant for each channel R,G,B,A, SRC after load
////constnat for each channel R,G,B,A, DST after load
//
//
////color = color_blend_op(src_load_coeficient.rgb * src_color_factor(a, b) * src_alpha_factor(a, b),
////                       dst_load_coeficient.rgb * dst_color_factor(a, b) * dst_alpha_factor(a, b));
////alpha = alpha_blend_op(src_load_coeficient.a * src_alpha_op_factor(a, b),
////                       dst_load_coeficient.a * alpha_blend_op(a, b));
//struct BlendData{
//    BlendOp src_color_factor;//4 bits for each, so total is 32bits, or 8 bits for each, so total is 64 bits.
//    BlendOp dst_color_factor;
//    BlendOp src_alpha_factor;
//    BlendOp dst_alpha_factor;
//    BlendOp src_alpha_op_factor;
//    BlendOp dst_alpha_op_factor;
//    BlendOp color_blend_op;
//    BlendOp alpha_blend_op;
//    vec4 src_load_coeficient;
//    vec4 dst_load_coeficient;
//    //fill with alternative but could just have multiple blend datas to do the same? or hardcode said blend stuff into type.
//    //ie alpha < 0.5 do x. else do y.
//    //but would also need per channel check for color.
//};
//
////implicit
//
////16 slots if 4 bytes
//
//// src fctor, dst factor, scr alpha  factor, dst alpha factor,
//// alpha src alpha factor, dst src alpha factor.
////color blend,
////alpha blend,
//
//
//// blend factor src, blend factor dst.
//// blend factor src alpha, blend factor dst alpha .
//// blend op for color
//// blend op for alpha
//// blend factor for src alpha, blend factor for dst alpha,
//// blend op for alpha and src
//// blend op for alpha and dst
//
//struct BlendFunctionType{
//    uint value;
//};
//

struct BlendOverlap{
    uint8_t value;
};

const BlendOverlap BLEND_OVERLAP_UNCORRELATED = uint8_t(0u);
const BlendOverlap BLEND_OVERLAP_DISJOINT = uint8_t(1u);
const BlendOverlap BLEND_OVERLAP_CONJOINT = uint8_t(2u);

struct IntersectionCoefficientBits{
    uint8_t value;
};

const IntersectionCoefficientBits INTERSECTION_COEFFICIENT_SRC_AND_DST_BIT = uint8_t(4u);//X
const IntersectionCoefficientBits INTERSECTION_COEFFICIENT_SRC_INV_DST_BIT = uint8_t(1u);//Y
const IntersectionCoefficientBits INTERSECTION_COEFFICIENT_DST_INV_SRC_BIT = uint8_t(2u);//Z

vec3 get_overlap_coefficients(BlendOverlap overlap, float alpha_src, float alpha_dst){
    switch (overlap.value){
        case BLEND_OVERLAP_UNCORRELATED.value:{
            float p0 = alpha_src * alpha_dst;//Ab
            float p1 = alpha_src * (1.0 - alpha_dst);//As
            float p2 = alpha_dst * (1.0 - alpha_src);//Ad
            return vec3(p0, p1, p2);
        }
        case BLEND_OVERLAP_DISJOINT.value:{
            float p0 = max(alpha_src + alpha_dst - 1.0, 0.0);//Ab
            float p1 = min(alpha_src, (1.0 - alpha_dst));//As
            float p2 = min(alpha_dst, (1.0 - alpha_src));//Ad
            return vec3(p0, p1, p2);
        }
        case BLEND_OVERLAP_CONJOINT.value:{
            float p0 = min(alpha_src, alpha_dst);//Ab
            float p1 = min(alpha_src - alpha_dst, 0.0);//As
            float p2 = min(alpha_dst - alpha_src, 0.0);//Ad
            return vec3(p0, p1, p2);
        }
    }
}

vec3 get_intersection_coefficients(IntersectionCoefficientBits intersection){
    vec3 result = vec3(0.0);
    if (intersection.value & INTERSECTION_COEFFICIENT_SRC_AND_DST_BIT.value){
        result.x = 1.0;
    }
    if (intersection.value & INTERSECTION_COEFFICIENT_SRC_INV_DST_BIT.value){
        result.y = 1.0;
    }
    if (intersection.value & INTERSECTION_COEFFICIENT_DST_INV_SRC_BIT.value){
        result.z = 1.0;
    }
    return result;
}

struct BlendOp{
    uint8_t value;
};
const BlendOp BLEND_OP_ZERO                 = BlendOp(uint8_t( 0u));// 0
const BlendOp BLEND_OP_ONE                  = BlendOp(uint8_t( 1u));// 1
const BlendOp BLEND_OP_SRC                  = BlendOp(uint8_t( 2u));// a
const BlendOp BLEND_OP_DST                  = BlendOp(uint8_t( 3u));// a
const BlendOp BLEND_OP_ADD                  = BlendOp(uint8_t( 4u));// a + b
const BlendOp BLEND_OP_SUB                  = BlendOp(uint8_t( 5u));// a - b
const BlendOp BLEND_OP_MUL                  = BlendOp(uint8_t( 6u));// a * b
const BlendOp BLEND_OP_DIV                  = BlendOp(uint8_t( 7u));// a / b
const BlendOp BLEND_OP_DIFF                 = BlendOp(uint8_t( 8u));// abs(a - b)
const BlendOp BLEND_OP_MIN                  = BlendOp(uint8_t( 9u));// min(a,b)
const BlendOp BLEND_OP_MAX                  = BlendOp(uint8_t(10u));// max(a,b)
const BlendOp BLEND_OP_ONE_MINUS_MUL        = BlendOp(uint8_t(11u));// 1 - (a * b)
const BlendOp BLEND_OP_ONE_MINUS_DIV        = BlendOp(uint8_t(12u));// 1 - (a / b)
const BlendOp BLEND_OP_SUM_ADD_MUL          = BlendOp(uint8_t(13u));// a + b + a*b
const BlendOp BLEND_OP_SUM_SUB_MUL          = BlendOp(uint8_t(13u));// a + b - a*b
const BlendOp BLEND_OP_SCREEN               = BlendOp(uint8_t(14u));// 1 - (1-a)(1-b)
const BlendOp BLEND_OP_EXCLUSION            = BlendOp(uint8_t(15u));// a + b - 2*a*b
const BlendOp BLEND_OP_OVERLAY              = BlendOp(uint8_t(16u));// overlay
const BlendOp BLEND_OP_HARDLIGHT            = BlendOp(uint8_t(17u));// hardlight
const BlendOp BLEND_OP_SOFTLIGHT_PHOTOSHOP  = BlendOp(uint8_t(18u));// softlight
const BlendOp BLEND_OP_SOFTLIGHT_VULKAN     = BlendOp(uint8_t(19u));// softlight
const BlendOp BLEND_OP_COLOR_DODGE          = BlendOp(uint8_t(20u));// color dodge
const BlendOp BLEND_OP_COLOR_BURN           = BlendOp(uint8_t(21u));// color burn
const BlendOp BLEND_OP_LINEAR_DODGE         = BlendOp(uint8_t(22u));// linear dodge
const BlendOp BLEND_OP_LINEAR_BURN          = BlendOp(uint8_t(23u));// linear burn
const BlendOp BLEND_OP_VIVIDLIGHT           = BlendOp(uint8_t(24u));// vivid light
const BlendOp BLEND_OP_LINEARLIGHT          = BlendOp(uint8_t(25u));// linear light
const BlendOp BLEND_OP_PINLIGHT             = BlendOp(uint8_t(26u));// pin light
const BlendOp BLEND_OP_HARDMIX              = BlendOp(uint8_t(27u));// hard mix
const BlendOp BLEND_OP_HSL_HUE              = BlendOp(uint8_t(28u));// HSL hue
const BlendOp BLEND_OP_SATURATION_HUE       = BlendOp(uint8_t(29u));// HSL saturation
const BlendOp BLEND_OP_HSL_COLOR            = BlendOp(uint8_t(30u));// HSL color
const BlendOp BLEND_OP_HSL_LUMINOSITY       = BlendOp(uint8_t(31u));// HSL luminosity


struct BlendLoadOp{
    uint8_t value;
};


const BlendLoadOp BLEND_LOAD_OP_SWAP = BlendLoadOp(uint8_t(1u));// 0
const BlendLoadOp BLEND_LOAD_OP_ONE_MINUS_SRC = BlendLoadOp(uint8_t(2u));// 0
const BlendLoadOp BLEND_LOAD_OP_ONE_MINUS_DST = BlendLoadOp(uint8_t(4u));// 0

void apply_load_op(inout vec3 color_src, inout vec3 color_dst, const in BlendLoadOp blend_load_op){
    if (blend_load_op.value & BLEND_LOAD_OP_ONE_MINUS_SRC.value){
        color_src = 1.0 - color_src;
    }
    if (blend_load_op.value & BLEND_LOAD_OP_ONE_MINUS_DST.value){
        color_dst = 1.0 - color_dst;
    }
    if (blend_load_op.value & BLEND_LOAD_OP_SWAP.value){
        vec3 temp = color_src;
        color_src = color_dst;
        color_dst = temp;
    }
}





float minv3(vec3 c) {
    return min(min(c.r, c.g), c.b);
}
float maxv3(vec3 c) {
    return max(max(c.r, c.g), c.b);
}
float lumv3(vec3 c) {
    return dot(c, vec3(0.30, 0.59, 0.11));
}
float satv3(vec3 c) {
    return maxv3(c) - minv3(c);
}

// If any color components are outside [0,1], adjust the color to
// get the components in range.
vec3 ClipColor(vec3 color) {
    float lum = lumv3(color);
    float mincol = minv3(color);
    float maxcol = maxv3(color);
    if (mincol < 0.0) {
        color = lum + ((color-lum)*lum) / (lum-mincol);
    }
    if (maxcol > 1.0) {
        color = lum + ((color-lum)*(1-lum)) / (maxcol-lum);
    }
    return color;
}

// Take the base RGB color <cbase> and override its luminosity
// with that of the RGB color <clum>.
vec3 SetLum(vec3 cbase, vec3 clum) {
    float lbase = lumv3(cbase);
    float llum = lumv3(clum);
    float ldiff = llum - lbase;
    vec3 color = cbase + vec3(ldiff);
    return ClipColor(color);
}

// Take the base RGB color <cbase> and override its saturation with
// that of the RGB color <csat>.  The override the luminosity of the
// result with that of the RGB color <clum>.
vec3 SetLumSat(vec3 cbase, vec3 csat, vec3 clum)
{
    float minbase = minv3(cbase);
    float sbase = satv3(cbase);
    float ssat = satv3(csat);
    vec3 color;
    if (sbase > 0) {
        // Equivalent (modulo rounding errors) to setting the
        // smallest (R,G,B) component to 0, the largest to <ssat>,
        // and interpolating the "middle" component based on its
        // original value relative to the smallest/largest.
        color = (cbase - minbase) * ssat / sbase;
    } else {
        color = vec3(0.0);
    }
    return SetLum(color, clum);
}

vec3 apply_blend_op(vec3 color_src, vec3 color_dst, BlendOp blend_op, BlendLoadOp load_op){
    apply_load_op(color_src, color_dst, load_op);
    switch (blend_op.value){
        case BLEND_OP_ZERO.value : {
            return vec3(0.0);
        }
        case BLEND_OP_ONE.value : {
            return vec3(1.0);
        }
        case BLEND_OP_SRC.value : {
            return color_src;
        }
        case BLEND_OP_DST.value : {
            return color_dst;
        }
        case BLEND_OP_ADD.value : {
            return color_src + color_dst;
        }
        case BLEND_OP_SUB.value : {
            return color_src - color_dst;
        }
        case BLEND_OP_MUL.value : {
            return color_src * color_dst;
        }
        case BLEND_OP_DIV.value : {
            return color_src / color_dst;
        }
        case BLEND_OP_DIFF.value : {
            return abs(color_src - color_dst);
        }
        case BLEND_OP_MIN.value : {
            return min(color_src, color_dst);
        }
        case BLEND_OP_MAX.value : {
            return max(color_src, color_dst);
        }
        case BLEND_OP_ONE_MINUS_MUL.value : {
            return vec3(1.0) - color_src * color_dst;
        }
        case BLEND_OP_ONE_MINUS_DIV.value : {
            return vec3(1.0) - color_src / color_dst;
        }
        case BLEND_OP_SUM_ADD_MUL.value : {
            return (color_src + color_dst) + (color_src * color_dst);
        }
        case BLEND_OP_SUM_SUB_MUL.value : {
            return (color_src + color_dst) - (color_src * color_dst);
        }
        case BLEND_OP_SCREEN.value : {
            return vec3(1.0) - ((1.0 - color_src) * (1.0 - color_dst));
        }
        case BLEND_OP_EXCLUSION.value : {
            return color_src + color_dst - (2.0 * color_src * color_dst);
        }
        case BLEND_OP_OVERLAY.value : {

            return ternary(lessThan(color_dst, 0.5),
                           2.0 * color_src * color_dst,
                           1.0 - 2.0 * (1.0 - color_src) * (1.0 - color_dst));
        }
        case BLEND_OP_HARDLIGHT.value : {
            return ternary(lessThan(color_src, 0.5),
                            2.0 * color_src * color_dst,
                            1.0 - 2.0 * (1.0 - color_src) * (1.0 - color_dst));
        }
        case BLEND_OP_SOFTLIGHT_PHOTOSHOP.value : {
            return ternary(lessThan(color_src, 0.5),
                            2.0*color_src*color_dst + color_src*color_src * (1.0 - 2.0 * color_dst),
                            2.0*color_src*(1.0 - color_dst) + sqrt(color_src) * (2.0 * color_dst - 1.0));
        }
        case BLEND_OP_SOFTLIGHT_VULKAN.value : {
            return ternary(lessThan(color_src, 0.5),
                           color_dst - (1.0 - 2.0 * color_src) * color_dst * (1.0 - color_dst),
                   ternary(greaterThan(color_src, 0.5) && lessThanEqual(color_dst, 0.25),
                           color_dst + (2.0 * color_src - 1.0) * color_dst * ((16.0 * color_dst - 12.0) * color_dst + 3.0),
                 //ternary(greaterThan(color_src, 0.5) && greaterThan(color_dst, 0.25),
                           color_dst + (2.0 * color_src - 1.0) * (sqrt(color_dst) - color_dst)
                   )
            );
        }
        case BLEND_OP_COLOR_DODGE.value : {
            return ternary(lessThanEqual(color_dst, 0.0),
                           vec3(0.0),
                           ternary(greaterThanEqual(color_src, 1.0),  vec3(1.0),  min(1.0,  (color_dst) / (1.0 - color_src)))
            );
        }
        case BLEND_OP_COLOR_BURN.value : {
            return ternary(greaterThanEqual(color_dst, 1.0), vec3(1.0),
                           ternary(lessThanEqual(color_src, 0.0),  vec3(0.0),
                                                                     1.0 - min(1.0,  (1.0 - color_dst) / (color_src)))
            );
        }
        case BLEND_OP_LINEAR_DODGE.value : {
            return ternary(lessThanEqual(color_src + color_dst, 1.0),
                           color_src + color_dst,
                           vec3(0.0)
            );
        }
        case BLEND_OP_LINEAR_BURN.value : {
            return ternary(greaterThan(color_src + color_dst, 1.0),
                          (color_src + color_dst) - 1.0,
                          vec3(0.0)
            );
        }

        case BLEND_OP_VIVIDLIGHT :{
            return ternary(greaterThanEqual(color_src, 0.0) && lessThan(color_src, 0.5),
                           1.0 - min(1.0, (1.0 - color_dst) / (2.0 * color_src)),
                   ternary(lessThanEqual(color_src, 0.0),
                           vec3(0.0),
                   ternary(greaterThanEqual(color_src, 0.5) && lessThan(color_src, 1.0),
                           min(1.0, (color_dst) / (2.0 * (1.0 - color_src))),
                           vec3(1.0)
                           )
                       )
                   );
        }
        case BLEND_OP_LINEARLIGHT :{
            vec3 temp = 2.0 * color_src + color_dst;
            return ternary(greaterThan(temp, 2.0),
                           vec3(1.0),
                   ternary(greaterThan(temp, 1.0) && lessThanEqual(temp, 2.0),
                           2.0 * color_src + color_dst - 1.0,
                           vec3(0.0)
                   )
            );
        }
        case BLEND_OP_PINLIGHT :{
            vec3 temp = 2.0 * color_src - 1.0;
            return ternary(greaterThanEqual(temp, color_dst) && lessThan(color_src, 0.5),
                           vec3(0.0),
                   ternary(greaterThanEqual(temp, color_dst) && greaterThanEqual(color_src, 0.5),
                           2.0 * color_src - 1.0,
                   ternary(lessThanEqual(temp, color_dst) && lessThan(color_src, 0.5 * color_dst),
                           2.0 * color_src,
                           color_dst
                           )
                       )
           );
        }
        case BLEND_OP_HARDMIX :{
            vec3 temp = 2.0 * color_src - 1.0;
            return ternary(lessThan(temp, 1.0),
                    vec3(0.0),
                    vec3(1.0)
            );
        }
        case BLEND_OP_HSL_HUE.value : {
            return SetLumSat(color_src, color_dst, color_src);
        }
        case BLEND_OP_SATURATION_HUE.value : {
            return SetLumSat(color_dst, color_src, color_dst);
        }
        case BLEND_OP_HSL_COLOR.value : {
            return SetLum(color_src, color_dst);
        }
        case BLEND_OP_HSL_LUMINOSITY.value : {
            return SetLum(color_dst, color_src);
        }
    }
}

void apply_load_op(inout float color_src, inout float color_dst, const in BlendLoadOp blend_load_op){
    if (blend_load_op.value & BLEND_LOAD_OP_ONE_MINUS_SRC.value){
        color_src = 1.0 - color_src;
    }
    if (blend_load_op.value & BLEND_LOAD_OP_ONE_MINUS_DST.value){
        color_dst = 1.0 - color_dst;
    }
    if (blend_load_op.value & BLEND_LOAD_OP_SWAP.value){
        float temp = color_src;
        color_src = color_dst;
        color_dst = temp;
    }
}

float apply_blend_op(float color_src, float color_dst, BlendOp blend_op, BlendLoadOp load_op){
    apply_load_op(color_src, color_dst, load_op);
    switch (blend_op.value){
        case BLEND_OP_ZERO.value : {
            return float(0.0);
        }
        case BLEND_OP_ONE.value : {
            return float(1.0);
        }
        case BLEND_OP_SRC.value : {
            return color_src;
        }
        case BLEND_OP_DST.value : {
            return color_dst;
        }
        case BLEND_OP_ADD.value : {
            return color_src + color_dst;
        }
        case BLEND_OP_SUB.value : {
            return color_src - color_dst;
        }
        case BLEND_OP_MUL.value : {
            return color_src * color_dst;
        }
        case BLEND_OP_DIV.value : {
            return color_src / color_dst;
        }
        case BLEND_OP_DIFF.value : {
            return abs(color_src - color_dst);
        }
        case BLEND_OP_MIN.value : {
            return min(color_src, color_dst);
        }
        case BLEND_OP_MAX.value : {
            return max(color_src, color_dst);
        }
        case BLEND_OP_ONE_MINUS_MUL.value : {
            return float(1.0) - color_src * color_dst;
        }
        case BLEND_OP_ONE_MINUS_DIV.value : {
            return float(1.0) - color_src / color_dst;
        }
        case BLEND_OP_SUM_ADD_MUL.value : {
            return (color_src + color_dst) + (color_src * color_dst);
        }
        case BLEND_OP_SUM_SUB_MUL.value : {
            return (color_src + color_dst) - (color_src * color_dst);
        }
        case BLEND_OP_SCREEN.value : {
            return float(1.0) - ((1.0 - color_src) * (1.0 - color_dst));
        }
        case BLEND_OP_EXCLUSION.value : {
            return color_src + color_dst - (2.0 * color_src * color_dst);
        }
        case BLEND_OP_OVERLAY.value : {

            return ternary(lessThan(color_dst, 0.5),
            2.0 * color_src * color_dst,
            1.0 - 2.0 * (1.0 - color_src) * (1.0 - color_dst));
        }
        case BLEND_OP_HARDLIGHT.value : {
            return ternary(lessThan(color_src, 0.5),
            2.0 * color_src * color_dst,
            1.0 - 2.0 * (1.0 - color_src) * (1.0 - color_dst));
        }
        case BLEND_OP_SOFTLIGHT_PHOTOSHOP.value : {
            return ternary(lessThan(color_src, 0.5),
            2.0*color_src*color_dst + color_src*color_src * (1.0 - 2.0 * color_dst),
            2.0*color_src*(1.0 - color_dst) + sqrt(color_src) * (2.0 * color_dst - 1.0));
        }
        case BLEND_OP_SOFTLIGHT_VULKAN.value : {
            return ternary(lessThan(color_src, 0.5),
            color_dst - (1.0 - 2.0 * color_src) * color_dst * (1.0 - color_dst),
            ternary(greaterThan(color_src, 0.5) && lessThanEqual(color_dst, 0.25),
            color_dst + (2.0 * color_src - 1.0) * color_dst * ((16.0 * color_dst - 12.0) * color_dst + 3.0),
            //ternary(greaterThan(color_src, 0.5) && greaterThan(color_dst, 0.25),
            color_dst + (2.0 * color_src - 1.0) * (sqrt(color_dst) - color_dst)
            )
            );
        }
        case BLEND_OP_COLOR_DODGE.value : {
            return ternary(lessThanEqual(color_dst, 0.0),
            float(0.0),
            ternary(greaterThanEqual(color_src, 1.0),  float(1.0),  min(1.0,  (color_dst) / (1.0 - color_src)))
            );
        }
        case BLEND_OP_COLOR_BURN.value : {
            return ternary(greaterThanEqual(color_dst, 1.0), float(1.0),
            ternary(lessThanEqual(color_src, 0.0),  float(0.0),
            1.0 - min(1.0,  (1.0 - color_dst) / (color_src)))
            );
        }
        case BLEND_OP_LINEAR_DODGE.value : {
            return ternary(lessThanEqual(color_src + color_dst, 1.0),
            color_src + color_dst,
            float(0.0)
            );
        }
        case BLEND_OP_LINEAR_BURN.value : {
            return ternary(greaterThan(color_src + color_dst, 1.0),
            (color_src + color_dst) - 1.0,
            float(0.0)
            );
        }

        case BLEND_OP_VIVIDLIGHT :{
            return ternary(greaterThanEqual(color_src, 0.0) && lessThan(color_src, 0.5),
            1.0 - min(1.0, (1.0 - color_dst) / (2.0 * color_src)),
            ternary(lessThanEqual(color_src, 0.0),
            float(0.0),
            ternary(greaterThanEqual(color_src, 0.5) && lessThan(color_src, 1.0),
            min(1.0, (color_dst) / (2.0 * (1.0 - color_src))),
            float(1.0)
            )
            )
            );
        }
        case BLEND_OP_LINEARLIGHT :{
            float temp = 2.0 * color_src + color_dst;
            return ternary(greaterThan(temp, 2.0),
            float(1.0),
            ternary(greaterThan(temp, 1.0) && lessThanEqual(temp, 2.0),
            2.0 * color_src + color_dst - 1.0,
            float(0.0)
            )
            );
        }
        case BLEND_OP_PINLIGHT :{
            float temp = 2.0 * color_src - 1.0;
            return ternary(greaterThanEqual(temp, color_dst) && lessThan(color_src, 0.5),
            float(0.0),
            ternary(greaterThanEqual(temp, color_dst) && greaterThanEqual(color_src, 0.5),
            2.0 * color_src - 1.0,
            ternary(lessThanEqual(temp, color_dst) && lessThan(color_src, 0.5 * color_dst),
            2.0 * color_src,
            color_dst
            )
            )
            );
        }
        case BLEND_OP_HARDMIX :{
            float temp = 2.0 * color_src - 1.0;
            return ternary(lessThan(temp, 1.0),
            float(0.0),
            float(1.0)
            );
        }
        case BLEND_OP_HSL_HUE.value : {
            return 1.0;
//            return SetLumSat(color_src, color_dst, color_src);
        }
        case BLEND_OP_SATURATION_HUE.value : {
            return 1.0;
//            return SetLumSat(color_dst, color_src, color_dst);
        }
        case BLEND_OP_HSL_COLOR.value : {
            return 1.0;
//            return SetLum(color_src, color_dst);
        }
        case BLEND_OP_HSL_LUMINOSITY.value : {
            return 1.0;
//            return SetLum(color_dst, color_src);
        }
    }
}

//src outside, destination outside, overlap, alpha.
//then need X,Y,Z bits.
//then need IntersectionCoefficientBits (2 bits)
// so 5 bits, 8bits, 8bits, 8bits, 8bits. Then do 8bits for load? (4 bits for load of
vec4 blend_composite(vec4 src, vec4 dst, uint8_t overlap_intersection_bits, uint16_t blend_load_bits,
                     BlendOp overlap_blend,
                     BlendOp src_outside_blend,
                     BlendOp dst_outside_blend,
                     BlendOp alpha_blend){
    IntersectionCoefficientBits alpha_intersection_bits = IntersectionCoefficientBits(overlap_intersection_bits & uint8_t(0x07));
    BlendOverlap overlap_bits = BlendOverlap((overlap_intersection_bits >> 3u)& uint8_t(0x3));

    BlendLoadOp overlap_load_op = BlendLoadOp(blend_load_bits & uint16_t(0xF));
    BlendLoadOp src_outside_load_op = BlendLoadOp((blend_load_bits >> 4u) & uint16_t(0xF));
    BlendLoadOp dst_outside_load_op = BlendLoadOp((blend_load_bits >> 8u) & uint16_t(0xF));
    BlendLoadOp alpha_load_op = BlendLoadOp((blend_load_bits >> 12u) & uint16_t(0xF));

    vec3 color_src = src.rgb;
    vec3 color_dst = dst.rgb;

    float alpha_src = src.a;
    float alpha_dst = dst.a;


    vec3 intersection_coefficients = get_intersection_coefficients(alpha_intersection_bits);
    vec3 overlap_coefficients = get_overlap_coefficients(overlap_bits, alpha_src, alpha_dst);

    vec3 color_blend_both = apply_blend_op(color_src,color_dst,overlap_blend,overlap_load_op);
    vec3 color_blend_src = apply_blend_op(color_src,color_dst,src_outside_blend,src_outside_load_op);
    vec3 color_blend_dst = apply_blend_op(color_src,color_dst,dst_outside_blend,dst_outside_load_op);


    vec3 color = color_blend_both * overlap_coefficients[0]
                 + color_blend_src * overlap_coefficients[1]
                 + color_blend_dst * overlap_coefficients[2];


    float porter_duff_alpha = intersection_coefficients[0] * overlap_coefficients[0]
                              + intersection_coefficients[1] * overlap_coefficients[1]
                              + intersection_coefficients[2] * overlap_coefficients[2];

    float alpha = porter_duff_alpha;
    if(alpha_load_op != BLEND_OP_ZERO){
        alpha = apply_blend_op(alpha_src, alpha_dst, alpha_blend, alpha_load_op);
    }

    return vec4(color / porter_duff_alpha, alpha);
}


vec4 blend_composite(vec4 src, vec4 dst, uint overlap_intersect_load_bits, uint blend_bits){
    uint8_t overlap_intersection_bits = uint8_t(overlap_intersect_load_bits & 0xFFu);
    uint16_t blend_load_bits = uint16_t((overlap_intersect_load_bits >> 8) & 0xFFFFu);
    BlendOp overlap_blend     = BlendOp(uint8_t(blend_bits & 0xFFu));
    BlendOp src_outside_blend = BlendOp(uint8_t((blend_bits >> 8u) & 0xFFu));
    BlendOp dst_outside_blend = BlendOp(uint8_t((blend_bits >> 16u) & 0xFFu));
    BlendOp alpha_blend       = BlendOp(uint8_t((blend_bits >> 24u) & 0xFFu));
    return blend_composite(src, dst, overlap_intersection_bits, blend_load_bits, overlap_blend, src_outside_blend, dst_outside_blend, alpha_blend);
}

#endif//BLEND_UTILS
