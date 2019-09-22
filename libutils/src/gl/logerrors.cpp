#include "../../include/gl/logerrors.h"
#include <array>
#include <iostream>
#include <GL/glew.h>

constexpr const char* GLEnumToString(GLenum p_e)
{
	return (p_e==GL_INVALID_ENUM)?"INVALID_ENUM":
		   (p_e==GL_INVALID_VALUE)?"INVALID_VALUE":
		   (p_e==GL_INVALID_OPERATION)?"INVALID_OPERATION":
		   (p_e==GL_INVALID_FRAMEBUFFER_OPERATION)?"INVALID_FRAMEBUFFER_OPERATION":
		   (p_e==GL_OUT_OF_MEMORY)?"OUT_OF_MEMORY":
		   (p_e==GL_STACK_UNDERFLOW)?"STACK_UNDERFLOW":
		   (p_e==GL_STACK_OVERFLOW)?"STACK_OVERFLOW":
		   "Unknown GLError.";
}

bool gl::LogErrors(const char* p_message)
{
	GLenum e=glGetError();
	std::cerr<<p_message<<": ";
	if (e==GL_NO_ERROR)
	{
		std::cerr<<"OK\n";
		return false;
	}
	while (e!=GL_NO_ERROR)
	{
		std::cerr<<"GL_"<<GLEnumToString(e);
		e=glGetError();
	}
	std::cerr<<std::endl;
	return true;
}
