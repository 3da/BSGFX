#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

namespace BSGFX
{

struct Rectangle
{
	Rectangle(float x = 0, float y = 0, float width = 0, float height = 0) :
		x(x), y(y), width(width), height(height) {}
	float x, y;
	float width, height;
	bool IsInside(float x, float y) const
	{
		const Rectangle &r = *this;
		return x >= r.x && y >= r.y && x < r.x + r.width && y < r.y + r.height;
	}
};

//! Defines a rectangle inside pixel image data.
struct PixelRectangle
{
	PixelRectangle(int x = 0, int y = 0, int width = 0, int height = 0) :
		x(x), y(y), width(width), height(height) {}
	inline PixelRectangle(const Rectangle &r) :
		x((int)r.x), y((int)r.y), width((int)r.width), height((int)r.height) {}
	int x, y;
	int width, height;
};

class Texture
{
public:
	Texture(const char *fileName);
	Texture(const char *data, unsigned size);

	bool IsLoaded() const {return loaded;}

	void Create(unsigned int width, unsigned int height, unsigned int index, float uv[]);

	const float *GetUV() const {
		return uv;
	}
	unsigned int GetWidth() const {
		return width;
	}
	unsigned int GetHeight() const {
		return height;
	}
	unsigned int GetIndex() const {
		return index;
	}

protected:
	unsigned int index;

	unsigned int width;
	unsigned int height;

	float uv[4];

	bool loaded;




};





}


#endif // TEXTURE_H_INCLUDED
