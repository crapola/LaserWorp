#include "../../include/gfx/window.h"
#include <iostream>
#include <stdexcept>

Window::Window(const char* p_title,int p_width,int p_height,Uint32 p_flags):
	_window(nullptr)
{
	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		std::string s("Unable to init SDL: ");
		s+=SDL_GetError();
		throw std::runtime_error(s);
	}
	SDL_Window* screen = SDL_CreateWindow(p_title,
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  p_width,
										  p_height,
										  p_flags);
	if(screen==nullptr)
	{
		std::string s("Unable to create window: ");
		s+=SDL_GetError();
		throw std::runtime_error(s);
	}
	_window=screen;
}

Window::~Window()
{
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

Window::operator SDL_Window* () const
{
	return _window;
}

SDL_Surface* Window::Surface() const
{
	return SDL_GetWindowSurface(_window);
}

SDL_Point Window::Size() const
{
	int w,h;
	SDL_GetWindowSize(_window,&w,&h);
	return {w,h};
}
