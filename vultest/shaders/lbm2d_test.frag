#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in uint test_color;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    if (test_color == 32){
        outColor = vec4(0.5, 0.5, 0.5, 1.0);
    } else if (test_color == 354){
        outColor = vec4(1.0, 0.5, 0.5, 1.0);
}else{
        outColor = vec4(0.0,0.0,0.0,1.0);
    }
}