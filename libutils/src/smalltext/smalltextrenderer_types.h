struct Character
{
	void SetColors(uint8_t front,uint8_t back);
	// Front and back colors
	GLushort colors;
	// Flags (Unused)
	GLubyte flags;
	// Character code
	GLubyte c;		//ivec2.x 24
	// Position in pixels
	GLshort x,y;	//ivec2.y
	//+2 padding
};
static_assert(sizeof(Character)==sizeof(GLint)*2,"Wrong size.");
