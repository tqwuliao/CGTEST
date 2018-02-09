#pragma once
#include "include\GL\glew.h"
#include "include\GLFW\glfw3.h"
#include <string>
class Texture
{
public:
	static unsigned char * loadHeightMap(const char * file);
	Texture(const char*);
	void GetSize(int & x, int & y);
	Texture(const Texture& temp);
	Texture();
	GLuint TextureID,width,height;
	std::string type,path,filename;
	int w,h;
	GLuint id() const;

	~Texture();
};

