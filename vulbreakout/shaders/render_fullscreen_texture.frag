#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform sampler u_sampler;
layout(set = 0, binding = 2) uniform texture2D u_materials[];
layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 out_color;

layout(push_constant) uniform PushConstantBlock {
    uint u_material_id;
};

void main(){
    vec4 color = texture(sampler2D(u_materials[u_material_id], u_sampler), texcoord);
//    out_color = vec4(1.0,1.0,1.0,1.0);
    out_color = vec4(vec3(color), 1.0);
}