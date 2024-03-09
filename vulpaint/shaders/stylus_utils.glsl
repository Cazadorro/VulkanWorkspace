#ifndef STYLUS_UTILS
#define STYLUS_UTILS
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable

struct StylusUpdate {
    float x;
    float y;
    float normal_pressure;
    float x_tilt;
    float y_tilt;
    float distance;
    float padding[10]; // tangental pressure?  Krita has 16 different stylus parameters. should we do the same here?
};

layout(buffer_reference, buffer_reference_align= 4 * 16, scalar) buffer StylusUpdate_ref {
    StylusUpdate data;
};


StylusUpdate mix(const in StylusUpdate v0, const in StylusUpdate v1, float t){
    StylusUpdate result;
    z.x = mix(v0.x, v0.x, t);
    z.y = mix(v0.y, v0.y, t);
    z.normal_pressure = mix(v0.normal_pressure, v0.normal_pressure, t);
    z.x_tilt = mix(v0.x_tilt, v0.x_tilt, t);
    z.y_tilt = mix(v0.y_tilt, v0.y_tilt, t);
    z.distance = mix(v0.distance, v0.distance, t);
    return result;
}

StylusUpdate get_interpolated_stylus_update(uint stylus_update_count, const in StylusUpdate_ref stylus_updates, uint tip_index, float spacing){
    if(stylus_update_count == 1){
        return stylus_updates.data[0];
    }
    int end_interp_idx = 0;
    int start_interp_idx = 1;
    float tip_distance = tip_index * spacing;
    if(stylus_update_count > 2){
        //binary search for stylus update with distance <= tip_distance.
        int low_idx = 0;
        int high_idx = int(stylus_update_count);
        while (low_idx < high_idx){
            //https://zhu45.org/posts/2018/Jan/12/how-to-write-binary-search-correctly/
            //http://coldattic.info/post/95/
            //https://stackoverflow.com/questions/504335/what-are-the-pitfalls-in-implementing-binary-search
            int mid_idx = low_idx + (high_idx - low_idx) / 2;
            //note it should be impossible for distance to be less than 0, and our tip distance should never be less than zero. \
            // int should save us from accidental underflow.
            if (stylus_updates.data[mid_idx].distance <= tip_distance) {
                low_idx = mid_idx + 1;
            } else { // stylus_updates.data[mid_idx].distance > tip_distance
                high_idx = mid_idx;
            }
        }
        //high should never be 0.
        end_interp_idx = high;
        start_interp_idx = high - 1;
    }
    float start_stylus_distance = stylus_updates.data[start_interp_idx].distance;
    float interpolant = tip_distance - start_stylus_distance;
    return mix(stylus_updates.data[start_interp_idx], stylus_updates.data[end_interp_idx], interpolant);
}

#endif //STYLUS_UTILS
