#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float u_time;
    float u_lbm_width;
    float u_lbm_height;
    float u_3;
} ubo;


vec4 jet(float normal_value)
{
    vec3 color;
    if(normal_value<0.0) normal_value = 0.0;
    if(normal_value>1.0) normal_value = 1.0;
    float v1 = 1.0/7.0;
    float v2 = 2.0/7.0;
    float v3 = 3.0/7.0;
    float v4 = 4.0/7.0;
    float v5 = 5.0/7.0;
    float v6 = 6.0/7.0;
    //compute color
    if(normal_value<v1)
    {
        float c = normal_value/v1;
        color.x = 70.*(1.-c);
        color.y = 70.*(1.-c);
        color.z = 219.*(1.-c) + 91.*c;
    }
    else if(normal_value<v2)
    {
        float c = (normal_value-v1)/(v2-v1);
        color.x = 0.;
        color.y = 255.*c;
        color.z = 91.*(1.-c) + 255.*c;
    }
    else if(normal_value<v3)
    {
        float c = (normal_value-v2)/(v3-v2);
        color.x =  0.*c;
        color.y = 255.*(1.-c) + 128.*c;
        color.z = 255.*(1.-c) + 0.*c;
    }
    else if(normal_value<v4)
    {
        float c = (normal_value-v3)/(v4-v3);
        color.x = 255.*c;
        color.y = 128.*(1.-c) + 255.*c;
        color.z = 0.;
    }
    else if(normal_value<v5)
    {
        float c = (normal_value-v4)/(v5-v4);
        color.x = 255.*(1.-c) + 255.*c;
        color.y = 255.*(1.-c) + 96.*c;
        color.z = 0.;
    }
    else if(normal_value<v6)
    {
        float c = (normal_value-v5)/(v6-v5);
        color.x = 255.*(1.-c) + 107.*c;
        color.y = 96.*(1.-c);
        color.z = 0.;
    }
    else
    {
        float c = (normal_value-v6)/(1.-v6);
        color.x = 107.*(1.-c) + 223.*c;
        color.y = 77.*c;
        color.z = 77.*c;
    }
    return vec4(color.r/255.0,color.g/255.0,color.b/255.0,1.0);
}

void main() {
    vec4 color = texture(texSampler, fragTexCoord);
    vec2 vel = color.xy;
    vec2 offset = vec2((1.0/ubo.u_lbm_width), (1.0/ubo.u_lbm_height));
    offset *= 1.0;
    vec2 offset_array[9] = vec2[9](
        vec2(-1.0, -1.0),
        vec2( 0.0, -1.0),
        vec2( 1.0, -1.0),

        vec2( 1.0,  0.0),
        vec2( 0.0,  0.0),
        vec2(-1.0,  0.0),

        vec2(-1.0,  1.0),
        vec2( 0.0,  1.0),
        vec2( 1.0,  1.0)
    );
    vec2 vel_nw = texture(texSampler, fragTexCoord + offset_array[0] * offset).xy;
    vec2 vel_nm = texture(texSampler, fragTexCoord + offset_array[1] * offset).xy;
    vec2 vel_ne = texture(texSampler, fragTexCoord + offset_array[2] * offset).xy;

    vec2 vel_ee = texture(texSampler, fragTexCoord + offset_array[3] * offset).xy;
    vec2 vel_mm = texture(texSampler, fragTexCoord + offset_array[4] * offset).xy;
    vec2 vel_ww = texture(texSampler, fragTexCoord + offset_array[5] * offset).xy;

    vec2 vel_sw = texture(texSampler, fragTexCoord + offset_array[6] * offset).xy;
    vec2 vel_sm = texture(texSampler, fragTexCoord + offset_array[7] * offset).xy;
    vec2 vel_se = texture(texSampler, fragTexCoord + offset_array[8] * offset).xy;

    float forward_diff = (((vel_sm.x - vel_mm.x) + (vel_se.x - vel_ee.x)) / 2.0f) -
    (((vel_ee.y - vel_mm.y) + (vel_se.y - vel_sm.y)) / 2.0f);
//    float forward_diff = (vel_sm.x - vel_nm.x) - (vel_ee.y - vel_ww.y);
    float vorticity = forward_diff * 100.0 * 100.0 * 100.0;
    float abs_vorticity = abs(vorticity);


    if(color == vec4(1.0)){
        outColor = vec4(1.0);
        return;
    }
    if(color == vec4(0.5)){
        outColor = vec4(0.5, 0.5, 0.5, 1.0);
        return;
    }
    if(color == vec4(0.7)){
        outColor = vec4(0.7, 0.7, 0.7, 1.0);
        return;
    }

    if(true){
        float vort = (vel_ww.y - vel_ee.y) - (vel_nm.x - vel_sm.x);
        vort *= 10.0;
        vort = clamp(vort, -1.0, 1.0);
        if(vort >= 0.0){
            outColor = vec4(1.0,1.0-abs(vort), 1.0-abs(vort), 1.0);
        }else{
            outColor = vec4(1.0-abs(vort),1.0-abs(vort), 1.0, 1.0);
        }
//        outColor = vec4(vort >= 0.0 ? 1.0 - abs(vort) : 1.0, vort < 0.0 ? 1.0 - abs(vort) : 1.0, 1.0, 1.0);
//        outColor = vec4(vort >= 0.0 ? 1.0 - abs(vort) : 1.0, vort < 0.0 ? 1.0 - abs(vort) : 1.0, 1.0, 1.0);
//        outColor = vec4(length(vel)/10.0, 0.0, 0.0, 1.0);
//        outColor = vec4(color.y, 1.0, color.z, 1.0);
        return;
    }

}