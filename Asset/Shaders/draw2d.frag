#version 450

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[16];

void main()
{
	vec4 texColor = v_Color;
	switch(int(v_TexIndex))
	{
		case 00: texColor *= texture(u_Textures[00], v_TexCoord * v_TilingFactor); break;
		case 01: texColor *= texture(u_Textures[01], v_TexCoord * v_TilingFactor); break;
		case 02: texColor *= texture(u_Textures[02], v_TexCoord * v_TilingFactor); break;
		case 03: texColor *= texture(u_Textures[03], v_TexCoord * v_TilingFactor); break;
		case 04: texColor *= texture(u_Textures[04], v_TexCoord * v_TilingFactor); break;
		case 05: texColor *= texture(u_Textures[05], v_TexCoord * v_TilingFactor); break;
		case 06: texColor *= texture(u_Textures[06], v_TexCoord * v_TilingFactor); break;
		case 07: texColor *= texture(u_Textures[07], v_TexCoord * v_TilingFactor); break;
		case 08: texColor *= texture(u_Textures[08], v_TexCoord * v_TilingFactor); break;
		case 09: texColor *= texture(u_Textures[09], v_TexCoord * v_TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], v_TexCoord * v_TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], v_TexCoord * v_TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], v_TexCoord * v_TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], v_TexCoord * v_TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], v_TexCoord * v_TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], v_TexCoord * v_TilingFactor); break;
	}
	color = texColor;
}
