#pragma once

#include "include\GL\glew.h"
#include "include\glm\glm.hpp"
#include <fstream>
#include <map>
#include <sstream>

class shader
{
public:
	shader();
	shader(const char * file1, const char * file2);
	shader(const char*,const char*,const char*);
	void run();
	void use() { run(); }
	void bind(const char* title, int index);
	void bind(const char* title, const GLfloat* mat);
	void bind(const char * title, const GLfloat * mat, const int index);
	void bind(const char* title, GLfloat x, GLfloat y, GLfloat z);
	void bind(const char* title, glm::vec2 vec);
	void bind(const char* title, glm::vec3 vec);
	void bindF(const char * title, float f);
	void setBool(const std::string& name, bool value) { bind(name.data(), value); };
	void setVec3(const std::string& name, const glm::vec3& vec) { bind(name.data(), vec); };
	void setMat4(const std::string& name, const glm::mat4& val) { bind(name.data(), (GLfloat*)&val); };
	void setInt(const std::string& name, const int val) { bind(name.data(), val); };
	~shader();
private:
	std::map<std::string,GLuint> entities;
	GLuint vertexShader, fragShader, program, geometryShader;
};

