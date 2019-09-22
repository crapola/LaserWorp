#include "../../include/gfx/context.h"
#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <string>

Context::Context(SDL_Window* p_screen):_context(nullptr)
{
	Uint32 flags=SDL_GetWindowFlags(p_screen);
	if (!(flags&SDL_WINDOW_OPENGL))
	{
		throw std::runtime_error("Can't create GLEW context, "
								 "missing SDL_WINDOW_OPENGL flag.");
	}
#ifndef NDEBUG
	// Set debug flag, so that we can use glDebugMessageCallback.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	_context=SDL_GL_CreateContext(p_screen);
	if (_context==nullptr)
	{
		throw std::runtime_error("SDL_GL_CreateContext returned null.");
	}
	GLenum err=glewInit();
	if (GLEW_OK!=err)
	{
		SDL_GL_DeleteContext(_context);
		std::string s("Glew init failed: ");
		const char* c=reinterpret_cast<const char*>(glewGetErrorString(err));
		s+=std::string(c);
		throw std::runtime_error(s);
	}
	std::cout<<"Using GLEW "<<glewGetString(GLEW_VERSION)<<'\n';
	std::cout<<"OpenGL version "<<glGetString(GL_VERSION)<<'\n';
}

Context::~Context()
{
	SDL_GL_DeleteContext(_context);
}
