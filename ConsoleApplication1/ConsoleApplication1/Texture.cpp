#include "Texture.h"
#include "SOIL.h"
#include <vector>
#include "CacheManager.h"
#include <iostream>

unsigned char* Texture::loadHeightMap(const char *file)
{
	static int _w, _h, _c;
	return SOIL_load_image(file, &_w, &_h, &_c, 3);
}

Texture::Texture(const char *file)
	:type(""),path(""),filename(file)
{
	//T1
	TextureID = SOIL_load_OGL_texture(file, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID,SOIL_FLAG_DDS_LOAD_DIRECT);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/*glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	SOIL_free_image_data(image);*/
	glBindTexture(GL_TEXTURE_2D, 0);
	//this->GetSize(w, h);
}

void Texture::GetSize(int &x,int &y)
{
	unsigned char* image = SOIL_load_image(this->filename.data(), &x, &y, 0, SOIL_LOAD_AUTO);
	this->width = x;
	this->height = y;
}

Texture::Texture(const Texture & temp)
{
	this->TextureID = temp.TextureID;
	this->width = temp.width;
	this->height = temp.height;
}

Texture::Texture()
{
}

GLuint Texture::id() const
{
	return this->TextureID;
}


Texture::~Texture()
{
	CacheManager<Texture>::Instance().releaseResource(filename);
}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		if (!image) std::cout << "image failed." << std::endl;
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}