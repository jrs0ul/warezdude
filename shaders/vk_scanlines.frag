#version 450

layout(location = 0) in vec2 vUvs;
layout(location = 1) in vec4 vColor;
layout(location = 2) in float vTime;

layout(binding = 0) uniform sampler2D uTexture;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    mat4 ModelViewProjection;
    float time;
    float screenHeight;
} pc;


void main(void)
{

    vec4 original = texture(uTexture, vUvs) * vColor;

    original *= (mod(vUvs.y, (1.0 / (pc.screenHeight * 2.0)) * 2.0) * 1.0 / (1.0 / (pc.screenHeight * 2.0)));

    outColor = clamp(original * 0.45, 0.0, 1.0);
}
