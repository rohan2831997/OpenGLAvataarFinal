#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h.txt>

#include"shaderClass.h"

class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;

	Texture(const char* filename, const char* type, GLuint slot);

	void texUnit(Shader& shader, const char* uniform, GLuint unit);

	void Bind();
	void unbind();
	void Delete();
};
#endif // !TEXTURE_CLASS_H
