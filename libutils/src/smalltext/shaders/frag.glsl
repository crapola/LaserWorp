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
