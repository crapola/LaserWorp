#include "../../include/gl/program.h"
#include <iostream>	// Errors
#include <vector> 	// Error string buffer

namespace gl
{
Program::Program():Identifiable(glCreateProgram())
{
	std::cout<<"+program id="<<_id<<"\n";
}

Program::~Program()
{
	std::cout<<"-program id="<<_id<<"\n";
	glDeleteProgram(_id);
}

void Program::Attach(GLuint p_s) const
{
	glAttachShader(_id,p_s);
}

void Program::Bind() const
{
	glUseProgram(_id);
}

void Program::Link() const
{
	glLinkProgram(_id);
	// Log linking error if any
	GLint result=GL_FALSE,logLength;
	glGetProgramiv(_id,GL_LINK_STATUS,&result);
	if (result!=GL_TRUE)
	{
		std::cerr<<"Linking error for program "<<_id<<"\n";
		glGetProgramiv(_id,GL_INFO_LOG_LENGTH,&logLength);
		std::vector<char> errBuf(logLength);
		glGetProgramInfoLog(_id,logLength,nullptr,errBuf.data());
		std::cerr<<errBuf.data()<<std::endl;
	}
}

void Program::Unbind()
{
	glUseProgram(0);
}

}
