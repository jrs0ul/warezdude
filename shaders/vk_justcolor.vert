#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 vColor;
uniform mat4 ModelViewProjection;

void main(void)
{
   gl_Position = ModelViewProjection * vec4(inPosition, 0, 1);
   vColor = inColor;
}
