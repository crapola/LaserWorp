#include <GL/glew.h>
#include <iostream>
namespace gl
{
static void GLAPIENTRY CallBack(
	GLenum/* p_source*/,
	GLenum/* p_type*/,
	GLuint/* p_id*/,
	GLenum/* p_severity*/,
	GLsizei/* p_length*/,
	const GLchar *p_message,
	const void*/* p_userParam*/)
{
	std::cerr<<"OpenGL error: "<<p_message<<'\n';
}

void InitDebugProc()
{
	GLint flag(0);
	glGetIntegerv(GL_CONTEXT_FLAGS,&flag);
	if (!(flag&GL_CONTEXT_FLAG_DEBUG_BIT))
	{
		std::cerr<<"InitDebugProc requires a debug context!\n";
		return;
	}
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(CallBack,nullptr);
}
}
