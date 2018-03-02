#include "Mesh.h"
#include <string>
#include <sstream>
using namespace std;


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<std::shared_ptr<Texture>> textures)

	:vertices(vertices),
	indices(indices),
	textures(textures), shininess(16.0f),
	VAO(0),
	VBO(0),
	EBO(0),
	opacityhas(false),
	normalhas(false)
{
	setup();
}

Mesh::Mesh(Vertex v[],int vl, GLuint g[],int gl) :
	VAO(0), VBO(0), EBO(0),
	opacityhas(false),
	normalhas(false),
	shininess(16.0f)
{
	vertices.resize(vl);
	for (int i = vl-1;i >= 0;i--)
	{
		vertices[i] = v[i];
	}
	indices.resize(gl);
	for (int i = gl-1;i >= 0;i--)
	{
		indices[i] = g[i];
	}
	
}

Mesh::Mesh()
{
}

void Mesh::Draw(shader& Shader)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
										  // Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = this->textures[i]->type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if (name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		else
			ss << "1";
		number = "material." + name + ss.str();

		Shader.bind(number.data(), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i]->id());
	}
	Shader.bind("dynamic", 1);
	Shader.bind("material.normalhas", this->normalhas);
	Shader.bind("material.opacity", this->opacityhas);
	Shader.bindF("material.shininess", this->shininess);
	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Draw(shader& Shader,int repeat)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
										  // Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = this->textures[i]->type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if (name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		else
			ss << "1";
		number = "material." + name + ss.str();

		Shader.bind(number.data(), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i]->id());
	}
	Shader.bind("dynamic", 1);
	Shader.bind("material.normalhas", this->normalhas);
	Shader.bind("material.opacity", this->opacityhas);
	Shader.bindF("material.shininess", this->shininess);
	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0,repeat);
	glBindVertexArray(0);
}

void Mesh::reMesh(Vertex v[], int vl)
{
	vertices.resize(vl);
	for (int i = vl - 1;i >= 0;i--)
	{
		vertices[i] = v[i];
	}
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
		&this->vertices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}


Mesh::~Mesh()
{
	for (int i = 0;i < textures.size();i++)
		textures[i].reset();
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
	glDeleteVertexArrays(1, &this->VAO);
}

void Mesh::setup()
{
	if (!this->VAO) {

		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
	}

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
		&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size()*sizeof(GLuint),
		&this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Position));
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, BIDS));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, WEIGHTS));
	glBindVertexArray(0);

}
