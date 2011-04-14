#include "BSGFX/Videomode.h"

#include "BSGFX/Screen.h"

#include <SDL.h>

namespace BSGFX
{

bool Videomode::Initialize(const char *caption)
{
	return BSGFX::Screen::Initialize(width, height, depth, fullscreen, caption);
}

std::vector<Videomode> Videomode::GetAvaible()
{
	std::vector<Videomode> modes;
	SDL_Rect **m;
	m = SDL_ListModes(0, SDL_FULLSCREEN/*|SDL_HWSURFACE*/);
	for (int i = 0; m[i]; i++)
	{
		modes.push_back(Videomode(m[i]->w, m[i]->h, 0, true));
	}
	return modes;
}



}
