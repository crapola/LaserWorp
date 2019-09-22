/*
Character data
	GLushort colors;	X_0
	GLubyte flags;		X_16
	GLubyte c;			X_24
	GLshort x;			Y_0
	GLshort y;			Y_16
*/
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
