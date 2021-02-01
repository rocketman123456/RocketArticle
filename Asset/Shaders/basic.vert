#version 410 core

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

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

void main()
{
	gl_Position = per_frame.projectionMatrix * per_frame.viewMatrix * per_batch.modelMatrix * vec4(a_Position, 1.0);
}
