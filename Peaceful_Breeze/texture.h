#ifndef TEXTURE_H
#define TEXTURE_H

#include "engine.h"
#include "stb_image.h"

class Texture
{
public:
	Texture(const char* filename, GLint filtering = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);
	~Texture();
	GLuint id = 0;
private:
	GLint width, height, channels;
};

#endif // !TEXTURE_H
