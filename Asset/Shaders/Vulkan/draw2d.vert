#version 450
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

layout(location = 0) out VS_OUT
{
	vec4 v_Color;
	vec2 v_TexCoord;
	float v_TexIndex;
	float v_TilingFactor;
} vs_out;

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
	vs_out.v_Color = a_Color;
	vs_out.v_TexCoord = a_TexCoord;
	vs_out.v_TexIndex = a_TexIndex;
	vs_out.v_TilingFactor = a_TilingFactor;
	gl_Position = PerFrame.projectionMatrix * PerFrame.viewMatrix * PerBatch.modelMatrix * vec4(a_Position.xyz, 1.0f);
}