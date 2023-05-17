#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler[];
layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 out_color;

layout (constant_id = 0) const int IMAGE_COUNT = 3;
layout(push_constant) uniform PushConstantBlock{
    uint u_frame_idx;
};

void main(){
    float gray_color = texture(texSampler[u_frame_idx % IMAGE_COUNT], texcoord).r;
    gray_color = log2(gray_color + 1);
    out_color = vec4(vec3(gray_color), 1.0);
}