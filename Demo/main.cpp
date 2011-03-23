#include "BSGFX.h"


#include <cstdlib>
#include <cstdio>

#include <cmath>

#include <GL/gl.h>
#include <SDL.h>
#undef main


int main(int argc, char *argv[])
{
	BSGFX::Initialize(BSGFX::IT_PNG);
	BSGFX::Screen::Initialize(640, 480, 32, false, "window");

	BSGFX::Texture txt[2];
	txt[0].Load("Plane1.png");
	txt[1].Load("font_0.pn");

	BSGFX::Drawing::Color(1,1,1,1);
	BSGFX::Drawing::ClearColor(0, 0, 0);

	BSGFX::Font font("font.fnt");
	if (!font.Init())
		exit(1);


	int x = -320;

	while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				exit(0);

		}

		x++;
		if (x>640+320)
			x = -320;

		BSGFX::Rectangle rect;
		rect.x = 420;
		rect.y = 100;
		rect.width = 15;
		rect.height = 15;
		/*BSGFX::Drawing::Color(1,1,1);
		BSGFX::Drawing::Rect(320,240,225,200, BSGFX::A_CENTER, M_PI/36, &txt);
		BSGFX::Drawing::Color(0,1,1, 0.5);
		BSGFX::Drawing::Rect(320,240,300,300, BSGFX::A_CENTER, M_PI/36);
		BSGFX::Drawing::Color(1,0,0, 0.5);*/
		//BSGFX::Drawing::Sprite(320, 240, &txt[1], 1, 1, M_PI/2, BSGFX::A_CENTER, &rect);
		//BSGFX::Drawing::Color(1,0,0);
		font.DrawText(630,470, L"Работает? Или же нет? :)", BSGFX::A_BOTTOM_RIGHT, 2, 300);
		//BSGFX::Drawing::Rect(320,240,300,300, BSGFX::A_LEFT, M_PI/36);
		//BSGFX::Drawing::Sprite(0,0, &txt);
		BSGFX::Drawing::Flush();
	}
	return 0;
}
