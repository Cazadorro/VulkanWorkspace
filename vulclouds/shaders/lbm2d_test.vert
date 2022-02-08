#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

//layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inColor;
//layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uint test_color;


const uint CUBE_FACE_BACK = 0;
const uint CUBE_FACE_LEFT = 1;
const uint CUBE_FACE_FRONT = 2;
const uint CUBE_FACE_RIGHT = 3;
const uint CUBE_FACE_UP = 4;
const uint CUBE_FACE_DOWN = 5;

vec3 create_cube_vertex(uint cube_vertex_index){
    uint quad_vert_idx = cube_vertex_index % 6;
    uint face_index = cube_vertex_index / 6;

    uint corner_idx = quad_vert_idx > 2 ? (quad_vert_idx - 1) % 4 : quad_vert_idx; //each corner 0 -> 3, assuming counter clockwise.
    // using bool to float conversions to get corner offsets, using normal vulkan clip space convention, 1 is down in y.
    vec3 vertex = vec3(float(corner_idx == 2 || corner_idx == 3), float(corner_idx == 1 || corner_idx == 2), 0.0);
    vertex *= 2;
    vertex -= 1; //renormalizing between -1 and 1.
    switch(face_index){
        case CUBE_FACE_BACK : {
            break;
        }
        case CUBE_FACE_LEFT : {
            //90 degree rotation CW, in xz plane
            vertex = vec3(vertex.z, vertex.y, -vertex.x);
            break;
        }
        case CUBE_FACE_FRONT : {
            //180 degree rotation CW, in xz plane
            vertex = vec3(-vertex.x, vertex.y, -vertex.z);
            break;
        }
        case CUBE_FACE_RIGHT : {
            //-90 degree rotation CCW, in xz plane
            vertex = vec3(-vertex.z, vertex.y, vertex.x);
            break;
        }
        case CUBE_FACE_UP : {
            //90 degree rotation CW, in xy plane
            vertex = vec3(vertex.x,-vertex.z, vertex.y);
            break;
        }
        case CUBE_FACE_DOWN : {
            //-90 degree rotation CCW, in xy plane
            vertex = vec3(vertex.x, vertex.z, -vertex.y);
            break;
        }
    }
    return vertex;
}


void main() {

    uint cube_vertex_index = gl_VertexIndex % 36;
    vec3 vertex = create_cube_vertex(cube_vertex_index);

//    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
//    fragColor = inColor;
//    fragTexCoord = inTexCoord;
    fragColor = vec3(0.0);
    fragTexCoord = vec2(0.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertex, 1.0);

    test_color = 354;
}
