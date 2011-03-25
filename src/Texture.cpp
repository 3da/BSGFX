#include "Texture.h"

#include <SDL.h>
#include <SDL_image.h>
#include <GL/GL.h>
#include <GL/glext.h>

namespace BSGFX
{


static void sSimpleFixAlphaChannel(SDL_Surface *image, const SDL_Rect &srcr)
{
	if (image->w < 2 || image->h < 2)
		return;

	if (SDL_LockSurface(image) != 0)
		return;

	Uint32 *pixels = (Uint32 *)image->pixels;
	long x, y;
	long w = image->w, h = image->h;
	const unsigned long alphaMask = image->format->Amask;
	const unsigned long colorMask =
		image->format->Rmask | image->format->Gmask | image->format->Bmask;

	long co = 0;
	for (y = 0; y < h; ++y)
	{
		for (x = 0; x < w; ++x)
		{
			if ((pixels[co] & alphaMask) == 0)
			{
				// iterate through 3x3 window around pixel
				long left = x - 1, right = x + 1, top = y - 1, bottom = y + 1;
				if (left < 0) left = 0;
				if (right >= w) right = w - 1;
				if (top < 0) top = 0;
				if (bottom >= h) bottom = h - 1;
				long x2, y2, colors = 0, co2 = top * w + left;
				long red = 0, green = 0, blue = 0;
				for (y2 = top; y2 <= bottom; ++y2)
				{
					for (x2 = left; x2 <= right; ++x2)
					{
						Uint32 px = pixels[co2++];
						if (px & alphaMask)
						{
							red += (px & image->format->Rmask) >> image->format->Rshift;
							green += (px & image->format->Gmask) >> image->format->Gshift;
							blue += (px & image->format->Bmask) >> image->format->Bshift;
							++colors;
						}
					}
					co2 += w - (right - left + 1);
				}
				if (colors > 0)
				{
					pixels[co] &= alphaMask;
					pixels[co] |= (red / colors) << image->format->Rshift;
					pixels[co] |= (green / colors) << image->format->Gshift;
					pixels[co] |= (blue / colors) << image->format->Bshift;
				}
			}
			++co;
		}
	}

	// if rect is smaller than texture, copy rightmost/bottom col/row (and pixel at w,h)
	// as is from the inner one so that linear sampling works like clamp_to_edge
	if (srcr.w < image->w)
	{
		for (y = 0; y < srcr.h; ++y)
			pixels[y * w + srcr.w] = pixels[y * w + srcr.w - 1];
	}
	if (srcr.h < image->h)
	{
		for (x = 0; x < srcr.w; ++x)
			pixels[srcr.h * w + x] = pixels[(srcr.h - 1) * w + x];
	}
	if (srcr.w < image->w && srcr.h < image->h)
		pixels[srcr.h * w + srcr.w] = pixels[(srcr.h - 1) * w + srcr.w - 1];

	SDL_UnlockSurface(image);
}

static inline unsigned long sNextPowerOfTwo(unsigned long input)
{
	--input;
	input |= input >> 16;
	input |= input >> 8;
	input |= input >> 4;
	input |= input >> 2;
	input |= input >> 1;
	return input + 1;
}

//! Loads and constructs an Image from given SDL_RWops object.
/*!
 * \param source data object to load image from.
 * \param deleteSource true if source should be deleted after loading
 *                     the image or false otherwise.
	static Image * LoadImage(SDL_RWops *source, bool deleteSource);

//! Loads a SDL_Surface from given file name.
/*!
 * \param fileName file name of image to load.
 * \return surface object if successful, or 0 if unsuccessful.
 */

//! Loads a SDL_Surface from given SDL_RWops object.
/*!
 * \param source data object to load image from.
 * \param deleteSource true if source should be deleted after loading
 *                     the image or false otherwise.
 * \return surface object if successful, or 0 if unsuccessful.
 */
static SDL_Surface * LoadSurface(SDL_RWops *source, bool deleteSource)
{
	return IMG_Load_RW(source, deleteSource ? 1 : 0);
}

//! Constructs an Image from given SDL_Surface.
/*!
 * \param surface surface to create Image from.
 * \param flags image flags to use when constructing image.
 *              \sa turska::IMAGE_FLAG
 * \param sourceRect optional source rectangle to crop from the surface,
 *                   or 0 to use the whole surface.
 */

static SDL_Surface *LoadSurface(const char *fileName)
{
	SDL_RWops *rw = SDL_RWFromFile(fileName, "rb");
	if (rw == 0)
		return 0;
	return LoadSurface(rw, true);
}



//! Creates an OpenGL texture from given SDL_Surface.
/*!
 * \param oTexture resulting OpenGL texture name, when successful.
 * \param oUV resulting top-left and bottom-right U,V coordinates
 *            in texture (4 floats), when successful.
 * \param surface surface to create texture from.
 * \param flags image flags to use when constructing image.
 *              \sa turska::IMAGE_FLAG
 * \param sourceRect optional source rectangle to crop from the surface,
 *                   or 0 to use the whole surface.
 * \return true if successful, or false if unsuccessful.
 */
static bool CreateTexture(unsigned long &oTexture, float *oUV,
						  SDL_Surface *surface, PixelRectangle *sourceRect = 0)
{
	unsigned long w, h;
	SDL_Surface *image;
	SDL_Rect area;
	SDL_Rect srcr;
	unsigned long savedFlags;
	unsigned char savedAlpha;
	unsigned long srcw, srch;

	if (sourceRect != 0)
	{
		srcw = sourceRect->width;
		srch = sourceRect->height;
	}
	else
	{
		srcw = surface->w;
		srch = surface->h;
	}

	// Use the surface width and height expanded to powers of 2.
	w = sNextPowerOfTwo(srcw);
	h = sNextPowerOfTwo(srch);
	oUV[0] = 0;                  // Min X
	oUV[1] = 0;                  // Min Y
	oUV[2] = ((GLfloat)srcw/* - 0.0001f*/) / w;  // Max X
	oUV[3] = ((GLfloat)srch/* - 0.0001f*/) / h;  // Max Y

	image = SDL_CreateRGBSurface(
				SDL_HWSURFACE | SDL_SRCALPHA,
				w, h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
				0x000000FF,
				0x0000FF00,
				0x00FF0000,
				0xFF000000
#else
				0xFF000000,
				0x00FF0000,
				0x0000FF00,
				0x000000FF
#endif
			);
	if (image == 0)
		return false;

	// Save the alpha blending attributes.
	savedFlags = surface->flags & (SDL_SRCALPHA | SDL_RLEACCELOK);
	savedAlpha = surface->format->alpha;
	if ((savedFlags & SDL_SRCALPHA) == SDL_SRCALPHA)
		SDL_SetAlpha(surface, 0, 0);

	// Copy the surface into the GL texture image.
	area.x = 0;
	area.y = 0;
	area.w = srcw;
	area.h = srch;
	if (sourceRect != 0)
	{
		srcr.x = sourceRect->x;
		srcr.y = sourceRect->y;
		srcr.w = sourceRect->width;
		srcr.h = sourceRect->height;
	}
	else
		srcr = area;
	SDL_BlitSurface(surface, &srcr, image, &area);

	sSimpleFixAlphaChannel(image, area);

	// Restore the alpha blending attributes.
	if ((savedFlags & SDL_SRCALPHA) == SDL_SRCALPHA)
		SDL_SetAlpha(surface, savedFlags, savedAlpha);

	// Create an OpenGL texture from the image.
	glGenTextures(1, (GLuint *)&oTexture);
	glBindTexture(GL_TEXTURE_2D, oTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image); // No longer needed.

	return true;
}

static bool CreateImage(Texture *txt, SDL_Surface *surface, PixelRectangle *sourceRect = 0)
{
	bool success;
	unsigned long texture;
	float uv[4];

	if (surface == 0)
		return false;

	success = CreateTexture(texture, uv, surface, sourceRect);
	if (!success)
		return false;



	if (sourceRect == 0)
		txt->Create(surface->w, surface->h, texture, uv);
	else
		txt->Create(sourceRect->width, sourceRect->height, texture, uv);
	return true;
}

static bool LoadImage(Texture *txt, SDL_RWops *source, bool deleteSource)
{
	SDL_Surface *surface;

	surface = LoadSurface(source, deleteSource);
	if (surface == 0)
		return false;

	CreateImage(txt, surface);
	SDL_FreeSurface(surface);

	return true;
}

Texture::Texture(const char *fileName)
{
	SDL_RWops *rw = SDL_RWFromFile(fileName, "rb");
	if (rw == 0)
	{
		loaded = false;
		return;
	}

	loaded = LoadImage(this, rw, true);
}

Texture::Texture(const char *data, unsigned int size)
{
	SDL_RWops *rw = SDL_RWFromMem((void*)data, size);
	if (rw == 0)
	{
		loaded = false;
		return;
	}
	loaded = LoadImage(this, rw, true);

}

void Texture::Create(unsigned int w, unsigned int h, unsigned int i, float _uv[])
{
	width = w;
	height = h;
	index = i;
	for (int i=0; i<4; i++)
		uv[i]=_uv[i];
}



}
