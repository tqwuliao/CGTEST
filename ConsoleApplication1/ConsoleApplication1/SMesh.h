#pragma once
#include <vector>

#include "include\GL\glew.h"
#include "include\GLFW\glfw3.h"
#include "include\glm\glm.hpp"
#include "Texture.h"
#include "shader.h"
#include "include\assimp\postprocess.h"
#include <memory>

struct SVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class SMesh
{
public:
	std::vector<SVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	float shininess;
	bool normalhas;
	bool opacityhas;
	int BaseID;
	SMesh(std::vector<SVertex> vertices, std::vector<GLuint> indices, std::vector<std::shared_ptr<Texture>> textures);
	SMesh(SVertex v[], int vl, GLuint g[], int gl);
	SMesh();
	virtual void Draw(shader& Shader);
	void Draw(shader & Shader, int repeat);
	void reMesh(SVertex v[], int vl);
	~SMesh();
	GLuint VAO, VBO, EBO;
	void setup();
protected:
	
	glm::mat4 transform;
};

