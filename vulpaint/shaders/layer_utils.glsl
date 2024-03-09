#ifndef LAYER_UTILS
#define LAYER_UTILS

struct LayerSettings{
    uint parent_group_id; //need to some how handle parent stuff and inheriting things off of that.
    uint16_t visible; //put things like "alpha lock" and such in here.
    uint16_t alpha_locked;
    float opacity;
    uint overlap_intersect_load_bits;
    uint blend_bits;  //5 * 4 bytes
    uint inherit_alpha_src;
    uint padding_1;
    uint padding_2; // 8 * 4 bytes
};

layout(buffer_reference, buffer_reference_align= 32, scalar) buffer LayerSettings_ref {
    LayerSettings data;
};


#endif //LAYER_UTILS
