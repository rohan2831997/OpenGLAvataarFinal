#include"texture.h"

Texture::Texture(const char* filename, const char* type, GLuint slot, GLenum format, GLenum pixelGL_TEXTURE_2D)
{
	Texture::type = type;

	//load texture
	int widthImg, heightImg, numColCh;
	unsigned char* bytes = stbi_load(filename, &widthImg, &heightImg, &numColCh, 0);
	//flips the image so that it appears right side up
	stbi_set_flip_vertically_on_load(true);

	//let openGL process textures
	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	//choose between GL_NEAREST or GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//How does the texture repeat itself
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// float flatcolor[]={1.0f,1.0f,1.0f,1.0f};
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelGL_TEXTURE_2D, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	//unload the image
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint tex0Uni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1f(tex0Uni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
