#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#include "basicreftypes.glsl"
layout(binding = 0) uniform sampler2D texSampler[];
layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 out_color;

layout (constant_id = 0) const int IMAGE_COUNT = 3;
layout(push_constant) uniform PushConstantBlock{
    uint u_frame_idx;
    uint u_other_value;
    float u_zoom_level;
    uint u_ant_grid_dim;
    uint8_array u_ant_grid;
};



void main(){
    uvec2 int_pos = uvec2(texcoord * float(u_ant_grid_dim));
    if(u_ant_grid.data[int_pos.y * u_ant_grid_dim + int_pos.x] == 1u){
        out_color = vec4(0.0,0.0,0.0,1.0);
    }else{
        out_color = vec4(1.0,1.0,1.0,1.0);
    }
}