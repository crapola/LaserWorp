#include "../../include/gl/buffer.h"
#include <iostream>//
namespace gl
{
Buffer::Buffer():Identifiable(0)
{
	glGenBuffers(1,&_id);
	std::cout<<"+buffer id="<<_id<<"\n";
}

Buffer::~Buffer()
{
	std::cout<<"-buffer id="<<_id<<"\n";
	glDeleteBuffers(1,&_id);
}

void Buffer::Bind(GLenum target) const
{
	glBindBuffer(target,_id);
}

}
