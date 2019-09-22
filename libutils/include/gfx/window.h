#pragma once
#include <SDL.h>
/*
SDL window.
SDL_Init and SDL_Quit are respectively called on construction and destruction.
*/
class Window
{
public:
	// 'flags' can be one or more SDL_WindowFlags.
	Window(const char* title,int width,int height,Uint32 flags=0);
	~Window();
	operator SDL_Window* () const;
	SDL_Point Size() const;
	SDL_Surface* Surface() const;
private:
	Window(const Window&)=delete;
	Window& operator=(const Window&)=delete;
	SDL_Window* _window;
};
