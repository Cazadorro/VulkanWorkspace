#version 460


layout (location = 0) out vec2 texcoord;
layout(push_constant) uniform PushConstantBlock{
    uint u_frame_idx;
    float u_zoom_level;
};
void main()
{
    texcoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(texcoord * 2.0f + -1.0f, 0.0f, 1.0f);
}