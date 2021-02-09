#version 410 core
layout (points) in;
layout (points, max_vertices = 1) out;

in VS_OUT
{
	vec4 v_Color;
	vec2 v_TexCoord;
	float v_TexIndex;
	float v_TilingFactor;
} gs_in[];

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;

void OutputPoint(int index)
{
	v_Color = gs_in[index].v_Color;
	v_TexCoord = gs_in[index].v_TexCoord;
	v_TexIndex = gs_in[index].v_TexIndex;
	v_TilingFactor = gs_in[index].v_TilingFactor;
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
}

void main()
{
	OutputPoint(0);
	//OutputPoint(1);
	//OutputPoint(2);
	EndPrimitive();
}