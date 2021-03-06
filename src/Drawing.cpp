#include <GL/GL.h>
#include <SDL.h>
#include <cmath>

#include "BSGFX/Drawing.h"
#include "BSGFX/Videomode.h"
#include "BSGFX/Screen.h"

#include "bin/spriteLogo.h"

namespace BSGFX
{

unsigned long Drawing::circleList[2];

Texture *Drawing::spriteLogo = 0;


void Drawing::Line(int x1, int y1, int x2, int y2, float width)
{
	glLineWidth(width);
	glBegin(GL_LINES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

void Drawing::Color(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void Drawing::ClearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 0.0);
}

void Drawing::Flush()
{
	glFlush();
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);
}

void Drawing::Rect(int x, int y, int width, int height, unsigned long flags, float rotation, const Texture *texture, const Rectangle *sourceRect)
{

	bool usingSrcArea = false;
	Rectangle srcArea;
	if (texture)
	{
		usingSrcArea = true;
		if (sourceRect)
			srcArea = *sourceRect;
		else
		{
			srcArea = Rectangle(0, 0, (float)texture->GetWidth(), (float)texture->GetHeight());
			usingSrcArea = false;
		}
	}

	glPushAttrib(GL_COLOR_BUFFER_BIT);

	float uv[4]= {0};

	if (texture)
	{
		glBindTexture(GL_TEXTURE_2D, texture->GetIndex());


		memcpy(uv, texture->GetUV(), 4 * sizeof(float));
		if (flags & A_FLIP_HORIZ)
		{
			uv[0] = texture->GetUV()[2];
			uv[2] = texture->GetUV()[0];
		}
		if (flags & A_FLIP_VERT)
		{
			uv[1] = texture->GetUV()[3];
			uv[3] = texture->GetUV()[1];
		}
	}

	float halfWidth = width*0.5f, halfHeight = height*0.5f;
	if (usingSrcArea)
	{
		float uvW = uv[2] - uv[0];
		float uvH = uv[3] - uv[1];
		uv[0] += uvW * srcArea.x / texture->GetWidth();
		uv[1] += uvH * srcArea.y / texture->GetHeight();
		uv[2] = uv[0] + uvW * srcArea.width / texture->GetWidth();
		uv[3] = uv[1] + uvH * srcArea.height / texture->GetHeight();
		//halfWidth = srcArea.width * 0.5f;
		//halfHeight = srcArea.height * 0.5f;
	}

	float pivotX = -halfWidth, pivotY = -halfHeight; // transform center adjusted to top-left (default)
	if (flags & A_RIGHT)
		pivotX = halfWidth;
	if (flags & A_HORIZ_CENTER)
		pivotX = 0;
	if (flags & A_BOTTOM)
		pivotY = halfHeight;
	if (flags & A_VERT_CENTER)
		pivotY = 0;

	float vx1[4], vy1[4], vx2[4], vy2[4];
	float *vx = vx1, *vy = vy1;
	vx[0] = -halfWidth - pivotX;
	vy[0] = -halfHeight - pivotY;
	vx[1] = halfWidth - pivotX;
	vy[1] = -halfHeight - pivotY;
	vx[2] = -halfWidth - pivotX;
	vy[2] = halfHeight - pivotY;
	vx[3] = halfWidth - pivotX;
	vy[3] = halfHeight - pivotY;
	if (rotation)
	{
		float c = cosf(rotation), s = sinf(rotation);
		// rotate current vertices to temp vertices
		vx2[0] = vx[0] * c - vy[0] * s;
		vy2[0] = vx[0] * s + vy[0] * c;
		vx2[1] = vx[1] * c - vy[1] * s;
		vy2[1] = vx[1] * s + vy[1] * c;
		vx2[2] = vx[2] * c - vy[2] * s;
		vy2[2] = vx[2] * s + vy[2] * c;
		vx2[3] = vx[3] * c - vy[3] * s;
		vy2[3] = vx[3] * s + vy[3] * c;
		// switch temp vertices to final ones
		vx = vx2;
		vy = vy2;
	}

	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(uv[0], uv[1]);
	glVertex2f(vx[0] + x, vy[0] + y);
	glTexCoord2f(uv[2], uv[1]);
	glVertex2f(vx[1] + x, vy[1] + y);
	glTexCoord2f(uv[0], uv[3]);
	glVertex2f(vx[2] + x, vy[2] + y);
	glTexCoord2f(uv[2], uv[3]);
	glVertex2f(vx[3] + x, vy[3] + y);
	glEnd();

	glPopAttrib();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Drawing::Sprite(int x, int y, Texture *texture, float scaleX, float scaleY, float rotation, unsigned long flags, const Rectangle *sourceRect)
{
	if (!texture)
		return;
	Rectangle rect;
	if (sourceRect)
		rect = *sourceRect;
	else
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = texture->GetWidth();
		rect.height = texture->GetHeight();
	}
	Rect(x, y, rect.width*scaleX, rect.height*scaleY, flags, rotation, texture, &rect);

}

void Drawing::Sprite()
{
	if (!spriteLogo)
	{
		spriteLogo = Texture::Load((char*)SPRITE_LOGO, SPRITE_LOGO_LEN);
		if (!spriteLogo)
			return;
	}
	Videomode mode = Screen::GetVideomode();
	Rect(0,0,mode.GetWidth(), mode.GetHeight(), A_TOP_LEFT, 0, spriteLogo);

}

void Drawing::CircleFilled(int x, int y, int radius/*, Texture *texture*/)
{
	//if (texture)
	//	glBindTexture(GL_TEXTURE_2D, texture->GetIndex());
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(radius, radius, 1);
	glCallList(circleList[0]);
	glPopMatrix();
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Drawing::Circle(int x, int y, int radius, float width)
{
	glLineWidth(width);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(radius, radius, 1);
	glCallList(circleList[1]);
	glPopMatrix();
}


void Drawing::InitializeCircle(unsigned int segments)
{
	Drawing::circleList[0] = glGenLists(1);
	Drawing::circleList[1] = glGenLists(1);
	double diff = M_PI*2.0f/(double)segments;

	glNewList(circleList[0], GL_COMPILE);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5,0.5);
	glVertex2d(0,0);
	for(double rad = 0; rad < M_PI*2; rad+=diff)
	{
		double xOff = cos(rad);
		double yOff = sin(rad);
		double xCoord = (xOff+1.0)/2.0;
		double yCoord = (yOff+1.0)/2.0;
		glTexCoord2f(xCoord, yCoord);
		glVertex2d(xOff, yOff);

	}
	glTexCoord2f(1,0);
	glVertex2d(1,0);
	glEnd();
	glEndList();

	glNewList(circleList[1], GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	for(double rad = 0; rad < M_PI*2; rad+=diff)
	{
		glVertex2d(sin(rad), cos(rad));
	}
	glEnd();
	glVertex2d(1,0);
	glEndList();
}



}





