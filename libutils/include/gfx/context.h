#pragma once
#include <SDL.h>
/*
Create a GLEW context for an SDL Window.
This is a debug context unless NDEBUG is defined.
*/
class Context
{
public:
	Context(SDL_Window*);
	~Context();
private:
	Context(const Context&)=delete;
	Context& operator=(const Context&)=delete;
	SDL_GLContext _context; // void*
};
