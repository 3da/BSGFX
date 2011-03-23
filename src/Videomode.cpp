#include "Videomode.h"

#include "Screen.h"

namespace BSGFX
{

bool Videomode::Initialize(const char *caption)
{
	return BSGFX::Screen::Initialize(width, height, depth, fullscreen, caption);
}


}
