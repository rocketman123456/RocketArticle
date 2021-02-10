#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;

layout(std140) uniform PerFrameConstants
{
    mat4 viewMatrix;		// 64 bytes
    mat4 projectionMatrix;	// 64 bytes
    vec4 camPos;            // 16 bytes
    int  numLights;         // 4 bytes
} PerFrame;					// total 148 bytes

layout(std140) uniform PerBatchConstants
{
	mat4 modelMatrix;		
} PerBatch;

void main()
{
    v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	gl_Position = PerFrame.projectionMatrix * PerFrame.viewMatrix * PerBatch.modelMatrix * vec4(a_Position, 1.0f);
}
