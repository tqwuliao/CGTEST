#pragma once

#include "include\GL\glew.h"
#include "include\glm\glm.hpp"
#include "include\glm\gtc\matrix_transform.hpp"
#include "include\glm\gtc\type_ptr.hpp"
#include <iostream>
#include <map>
#include <string>

#include "include\ft2build.h"
#include "include\freetype\freetype.h"
#include "shader.h"

#pragma comment(lib,"freetype28d.lib")

struct C_DEF {
	GLuint id;
	glm::ivec2 size;
	glm::ivec2 margin;
	GLuint offset;
};

class TextRenderer
{
public:
	static TextRenderer& Instance();
	static glm::vec3 textColors[10];
	TextRenderer();
	~TextRenderer();
	void draw(std::string str, glm::vec3 color, glm::mat4 projection);
	shader textShader;
private:
	GLuint VAO, VBO;
	std::map<char, C_DEF> CDEFS;
};

