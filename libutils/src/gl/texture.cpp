#include "../../include/gl/texture.h"
#include <iostream>

namespace gl
{
Texture::Texture():Identifiable(0)
{
	glGenTextures(1,&_id);
	std::cout<<"+texture id="<<_id<<"\n";
}

Texture::~Texture()
{
	std::cout<<"-texture id="<<_id<<"\n";
	glDeleteTextures(1,&_id);
}
}
