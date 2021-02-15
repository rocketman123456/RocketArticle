#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

//layout(location = 0) out VS_OUT
//{
layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out float v_TexIndex;
layout(location = 3) out float v_TilingFactor;
//} vs_out;

layout(binding = 0) uniform PerFrameConstants
{
    mat4 viewMatrix;		// 64 bytes
    mat4 projectionMatrix;	// 64 bytes
    vec4 camPos;            // 16 bytes
    ivec4 numLights;        // 16 bytes
} PerFrame;					// total 160 bytes

layout(binding = 1) uniform PerBatchConstants
{
	mat4 modelMatrix;		
} PerBatch;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	gl_Position = PerFrame.projectionMatrix * PerFrame.viewMatrix * PerBatch.modelMatrix * vec4(a_Position.xyz, 1.0f);
}