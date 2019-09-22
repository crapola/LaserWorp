constexpr char const* SHADERS_STR[3]={
// Vertex
// -----------------------------------------------------------------------------
R"(
#version 400
layout(location=0) in ivec2 chardata;

uniform vec2 resolution;

flat out uint charoffset;
flat out vec2 charsizendc;

out Color
{
	vec4 front;
	vec4 back;
} outColor;

// Convert pixel position to NDC according to resolution.
vec2 PixToNDC(in vec2 v)
{
	return vec2((2.0f*v.x)/resolution.x-1.0f, -(2.0f*v.y)/resolution.y+1.0f);
}

// Get character position.
vec2 GetPos()
{
	vec2 u=vec2(chardata.y&0xFFFF,chardata.y>>16&0xFFFF);
	return PixToNDC(u);
}

// Get character ASCII256 value.
uint GetChar()
{
	return chardata.x>>24&0xFF;
}

vec4 ExtractColor(uint c)
{
	float alpha=(c&3)/3.0f;
	float blue=(c>>2&3)/3.0f;
	float green=(c>>4&3)/3.0f;
	float red=(c>>6&3)/3.0f;
	return vec4(red,green,blue,alpha);
}

// Get flags.
uint GetFlags()
{
	return chardata.x>>16;
}

void main()
{
	charsizendc=vec2(8.f * 2.f/resolution.x,8.f * 2.f/resolution.y);
	charoffset=GetChar();
	vec2 position=GetPos();
	outColor.front=ExtractColor(chardata.x>>8&0xFF);
	outColor.back=ExtractColor(chardata.x&0xFF);
	gl_Position=vec4(position, 0.0, 1.0);
}
)",
// Geometry
// -----------------------------------------------------------------------------
R"(
#version 400
layout(points) in;
layout(triangle_strip,max_vertices=4) out;

flat in uint charoffset[];
flat in vec2 charsizendc[];
in vec4 frontColor[];

in Color
{
	vec4 front;
	vec4 back;
} inColor[];

out Color
{
	vec4 front;
	vec4 back;
} outColor;

out vec2 uv;

void main()
{
	const float tw=256.0f;

	vec2 uvc=vec2((charoffset[0]-32)/tw,0);
	const float uvx=1.f/tw;
	const float uvy=1.f;

	float sx=charsizendc[0].x;
	float sy=charsizendc[0].y;

	vec4 v=gl_in[0].gl_Position;

	outColor.front=inColor[0].front;
	outColor.back=inColor[0].back;

	gl_Position=v;
	uv=uvc+vec2(0,0);
	EmitVertex();

	gl_Position+=vec4(0,-sy,0,0);
	uv=uvc+vec2(0,uvy);
	EmitVertex();

	gl_Position+=vec4(sx,sy,0,0);
	uv=uvc+vec2(uvx,0);
	EmitVertex();

	gl_Position+=vec4(0,-sy,0,0);
	uv=uvc+vec2(uvx,uvy);
	EmitVertex();
}
)",
// Fragment
// -----------------------------------------------------------------------------
R"(
#version 400
uniform sampler2D tex;

in vec2 uv;

in Color
{
	vec4 front;
	vec4 back;
};

out vec4 outColor;

void main()
{
	float r=texture(tex,uv).r;
	outColor=r*front+(1-r)*back;
}
)"};
