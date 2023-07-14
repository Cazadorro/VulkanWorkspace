#version 460


layout (location = 0) out vec2 texcoord;

void main()
{
    // gl_VertexIndex == 0
    // vec2(0, 0)
    // gl_VertexIndex == 1
    // 1 << 1 == 2
    // 2 & 2 = 2
    // 1 & 2 = 0
    // vec2(2.0, 0.0)
    // gl_VertexIndex == 2
    // 4 & 2 = 0
    // 2
    // vec2(0.0, 2.0)
    //vec2(0,0), vec2(2.0, 0.0), vec2(0.0, 2.0)
    //vec2(2.0, 0.0)
//    texcoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    texcoord = vec2(gl_VertexIndex & 2, (gl_VertexIndex << 1) & 2);
    gl_Position = vec4(texcoord * 2.0f + -1.0f, 0.0f, 1.0f);
}