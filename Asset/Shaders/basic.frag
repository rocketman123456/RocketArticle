#version 410 core

layout(location = 0) out vec4 color;

layout(binding = 10, std140) uniform PerFrameConstants
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 camPos;
    int numLights;
} per_frame;

layout(binding = 11, std140) uniform PerBatchConstants
{
    mat4 modelMatrix;
} per_batch;

layout(location = 0) uniform vec4 u_Color;

void main()
{
	color = u_Color;
}