#include "BSGFX.h"


#include <SDL.h>
#include <SDL_image.h>
#include <GL/Gl.h>

namespace BSGFX
{


bool Initialize(unsigned long useImageTypes)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return false;
	}

	int flags = 0;
	if (useImageTypes&IT_JPG)
		flags|=IMG_INIT_JPG;
	if (useImageTypes&IT_PNG)
		flags|=IMG_INIT_PNG;
	if (useImageTypes&IT_TIF)
		flags|=IMG_INIT_TIF;

	if (IMG_Init(flags) != flags)
	{
		fprintf(stderr, "Unable to initialize SDL_image: %s\n", IMG_GetError());
		return false;
	}
	atexit(SDL_Quit);
	atexit(IMG_Quit);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);



    //glLineWidth(2);

    // This allows alpha blending of 2D textures with the scene.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}






}
