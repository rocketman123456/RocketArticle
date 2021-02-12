#version 450
layout(location = 0) out vec4 color;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in float v_TexIndex;
layout(location = 3) in float v_TilingFactor;

layout(binding = 3) uniform sampler2D u_Textures[16];

void main()
{
	vec4 texColor = v_Color;
	color = texColor;
}