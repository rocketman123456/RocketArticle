#version 410 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;
in float v_TilingFactor;

uniform vec4 u_Color;

void main()
{
	color = u_Color;
}