#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec4 v_Color;
	vec2 v_TexCoord;
	float v_TexIndex;
	float v_TilingFactor;
} gs_in[];

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out float v_TexIndex;
layout(location = 3) out float v_TilingFactor;

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
	OutputPoint(1);
	OutputPoint(2);
	EndPrimitive();
}