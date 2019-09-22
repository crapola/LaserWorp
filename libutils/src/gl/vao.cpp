#include "../../include/gl/vao.h"
#include <iostream>

namespace gl
{

Vao::Vao()
{
	glGenVertexArrays(1,&_id);
	std::cout<<"+VAO id="<<_id<<"\n";
}

Vao::~Vao()
{
	glDeleteVertexArrays(1,&_id);
	std::cout<<"-VAO id="<<_id<<"\n";
}

void Vao::Bind() const
{
	glBindVertexArray(_id);
}

void Vao::Unbind()
{
	glBindVertexArray(0);
}

}
