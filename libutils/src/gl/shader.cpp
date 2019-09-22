#include "../../include/gl/shader.h"
#include <iostream>	// Errors
#include <vector> 	// Error string buffer

namespace gl
{

Shader::Shader(GLenum p_type):Identifiable(glCreateShader(p_type))
{
	std::cout<<"+shader type="<<p_type<<" id="<<_id<<"\n";
}

Shader::~Shader()
{
	std::cout<<"-shader id="<<_id<<"\n";
	glDeleteShader(_id);
}

void Shader::Compile(const char* p_source) const
{
	std::cout<<"Compiling shader "<<_id<<"\n";
	const GLchar* ss=p_source;
	glShaderSource(_id,1,&ss,nullptr);
	glCompileShader(_id);
	// Log error if any
	GLint result=GL_FALSE;
	glGetShaderiv(_id,GL_COMPILE_STATUS,&result);
	if (result!=GL_TRUE)
	{
		std::cerr<<"Shader "<<_id<<" compilation failed.\n";
		std::cerr<<"Source: \n"<<p_source<<"\n";
		int logLength;
		glGetShaderiv(_id,GL_INFO_LOG_LENGTH,&logLength);
		std::vector<char> errorMessageBuf(logLength);
		glGetShaderInfoLog(_id,logLength,nullptr,errorMessageBuf.data());
		std::cerr<<errorMessageBuf.data()<<std::endl;
	}
}

void Shader::Compile(const std::string& p_source) const
{
	Compile(p_source.data());
}

}
