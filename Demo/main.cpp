#include "BSGFX.h"


#include <cstdlib>
#include <cstdio>

#include <cmath>

#include <GL/gl.h>
#include <SDL.h>
#undef main

BSGFX::Font *font;
BSGFX::Texture *txt;

void DrawScreen1();

int main(int argc, char *argv[])
{
	BSGFX::Initialize(BSGFX::IT_PNG);

	BSGFX::Screen::Initialize(640, 480, 32, false, "window");
	BSGFX::Drawing::InitializeCircle(32);

	txt = new BSGFX::Texture("Plane1.png");
	if (!txt->IsLoaded())
		exit(1);
	BSGFX::Drawing::Color(1,1,1,1);
	BSGFX::Drawing::ClearColor(1, 1, 1);

	font = new BSGFX::Font("font.fnt");
	if (!font->Init())
		exit(1);


	while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				exit(0);

		}

		DrawScreen1();

		BSGFX::Drawing::Flush();
	}
	return 0;
}

void DrawScreen1()
{
		/*BSGFX::Drawing::Color(1,1,1);
		BSGFX::Drawing::Rect(320,240,225,200, BSGFX::A_CENTER, M_PI/36, &txt);
		BSGFX::Drawing::Color(0,1,1, 0.5);
		BSGFX::Drawing::Rect(320,240,300,300, BSGFX::A_CENTER, M_PI/36);
		BSGFX::Drawing::Color(1,0,0, 0.5);*/
		//BSGFX::Drawing::Sprite(320, 240, &txt[1], 1, 1, M_PI/2, BSGFX::A_CENTER, &rect);
		//BSGFX::Drawing::Color(1,0,0);
		BSGFX::Drawing::Sprite();

		static float rotation = 0;
		rotation+=0.01;
		BSGFX::Drawing::Color(0,0,0);
		font->DrawText(10,10, L"BSGFX primitives", BSGFX::A_TOP_LEFT, 1);
		font->DrawText(10,50, L"Filled circle", BSGFX::A_TOP_LEFT, 0.7);
		font->DrawText(300,50, L"Sprite", BSGFX::A_TOP_LEFT, 0.7);
		BSGFX::Drawing::Color(1,0,0);
		BSGFX::Drawing::CircleFilled(60, 130, 50);
		BSGFX::Drawing::Color(1,1,1);
		BSGFX::Drawing::Sprite(330, 130, txt, 0.3,0.3, rotation, BSGFX::A_CENTER);


		//BSGFX::Drawing::Color(1,1,1);
		//BSGFX::Drawing::CircleFilled(100,100, 100);

}
