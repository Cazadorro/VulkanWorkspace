#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 0, location = 0) uniform sampler2D texSampler[];
layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 out_color;

layout (constant_id = 0) const int IMAGE_COUNT = 3;
layout(push_constant) uniform PushConstantBlock{
    uint u_frame_idx;
};

void main(){
    out_color = texture(texSampler[u_frame_idx % IMAGE_COUNT], texcoord);
}