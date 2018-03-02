#pragma once
#include <vector>

#include "include\GL\glew.h"
#include "include\GLFW\glfw3.h"
#include "include\glm\glm.hpp"
#include "Texture.h"
#include "shader.h"
#include "include\assimp\postprocess.h"
#include <memory>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec4 WEIGHTS;
	glm::ivec4 BIDS;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	float shininess;
	bool normalhas;
	bool opacityhas;
	int BaseID;
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<std::shared_ptr<Texture>> textures);
	Mesh(Vertex v[], int vl, GLuint g[], int gl);
	Mesh();
	virtual void Draw(shader& Shader);
	void Draw(shader & Shader, int repeat);
	void reMesh(Vertex v[], int vl);
	~Mesh();
	void setup();
protected:
	GLuint VAO, VBO, EBO;
	glm::mat4 transform;
};

