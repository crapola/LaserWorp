// std
#include <iostream>
// libs
#include "gfx/context.h"
#include "gl/debug.h"
// local
//#define DEBUG_LOOP
#include "genloop.hpp"
#include "app.h"
using namespace std;
int main(int,char**) try
{
#ifdef NDEBUG
	cout << "Release build." << endl;
#else
	cout << "Debug build." << endl;
#endif
	// Create window and context.
	Window window("LaserWorp",600,600,SDL_WINDOW_OPENGL);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	Context context(window);
	gl::InitDebugProc();
	SDL_GL_SetSwapInterval(1); // Vsync
	// Create application and start loop.
	std::cout<<"App size is "<<sizeof(App)<<" bytes.\n";
	App app(window);
	app.Init();
	auto updater=[&app]()->bool
	{
		return app.Update();
	};
	auto renderer=[&app](float p)
	{
		app.Render(p);
	};
	Loopy<64>(updater,renderer);
	// End.
	return EXIT_SUCCESS;
}
catch (const std::runtime_error& e)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error!",e.what(),0);
	return EXIT_FAILURE;
}
