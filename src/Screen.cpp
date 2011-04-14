#include "BSGFX/Screen.h"

#include <SDL.h>
#include <GL/GL.h>

namespace BSGFX
{

Videomode Screen::videomode;
std::string Screen::caption;


bool Screen::Initialize(unsigned int width, unsigned int height,
						unsigned int depth,
						bool fullscreen,
						const char *caption, bool vsync)
{
	Screen::videomode = Videomode();
	Screen::caption.clear();
	int flags = SDL_OPENGL;
	if (fullscreen)
		flags|=SDL_FULLSCREEN;

	//SDL_ShowCursor(false);

	SDL_WM_SetCaption(caption, caption);

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, (vsync)?1:0);

	if (SDL_SetVideoMode(width, height, depth, flags) == 0)
	{
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		return false;
	}

	if (SetupOpengl(width, height))
	{
		Screen::videomode = Videomode(width, height, depth, fullscreen);
		Screen::caption = caption;
		return true;
	}
	return false;
}

bool Screen::SetupOpengl(unsigned int width, unsigned int height)
{
	glClearColor(0, 0, 0, 1);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (GLdouble)width, (GLdouble)height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_ALPHA_TEST);

	return true;
}




};
