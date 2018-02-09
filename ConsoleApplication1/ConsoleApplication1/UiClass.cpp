#include "UiClass.h"
#include <iostream>

extern glm::mat4 Transform_2D;
int Scwidth, Scheight;

Vertex UIquad[] =
{
	// Positions              // Normals（占位符）   // TexCoords
	{glm::vec3(1.0f,  1.0f, 0) ,glm::vec3(1,1,1), glm::vec2(1.0f, 0.0f)}, 
	{glm::vec3(1.0f, -1.0f, 0) ,glm::vec3(0,1,1), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-1.0f,-1.0f, 0) ,glm::vec3(1,1,0), glm::vec2(0.0f, 1.0f)},
	{glm::vec3(-1.0f, 1.0f, 0) ,glm::vec3(1,1,0), glm::vec2(0.0f, 0.0f)}
};

GLuint UIquadIndices[] = { 3,2,1,3,1,0 };

UiClass::UiClass()
	:Mesh(),
	background("./resources/back.png"),
	opacity(1.0)
{
	this->background.GetSize(this->Geometry.width, this->Geometry.height);
	this->setGeometry(0, 0, 1200, 700);
	for(int i = 0;i < 4;i++)
		this->vertices.insert(this->vertices.end(), UIquad[i]);
	for (int i = 0;i < 6;i++)
		this->indices.insert(this->indices.end(), UIquadIndices[i]);
	this->textures.insert(this->textures.end(),std::shared_ptr<Texture>(&background));
	this->setup();
}

UiClass::UiClass(std::string bgName)
	:Mesh(),
	background(bgName.data()),
	opacity(1.0)
{
	this->background.GetSize(this->Geometry.width, this->Geometry.height);
	this->setGeometry(0, 0);
	for (int i = 0; i < 4; i++)
		this->vertices.insert(this->vertices.end(), UIquad[i]);
	for (int i = 0; i < 6; i++)
		this->indices.insert(this->indices.end(), UIquadIndices[i]);
	this->textures.insert(this->textures.end(), std::shared_ptr<Texture>(&background));
	this->setup();
}

UiClass::~UiClass()
{
	for (int i = 0;i < textures.size();i++)
		if(textures[i].get()) textures[i].reset();
}

void UiClass::Draw(shader& Shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]->id());
	// Draw mesh
	Shader.bind("Texture", 0);
	Shader.bind("scale", this->WorldSize);
	Shader.bind("offset", this->WorldPos);
	Shader.bindF("opacity", this->opacity);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void UiClass::setGeometry(int x, int y)
{
	this->Geometry.x = x;
	this->Geometry.y = y;

	this->WorldSize.x = this->Geometry.width * 1.0 / Scwidth;
	this->WorldSize.y = this->Geometry.height * 1.0 / Scheight;

	this->WorldPos.x = x * 1.0 / Scwidth;
	this->WorldPos.y = y * 1.0 / Scheight;

	//printf("%f %f\n", this->WorldPos.x, this->WorldPos.y);
	//printf("%f %f\n", this->WorldSize.x, this->WorldSize.y);
}

void UiClass::setGeometry(int x, int y, int _width, int _height)
{
	this->Geometry.x = x;
	this->Geometry.y = y;
	this->Geometry.width = _width;
	this->Geometry.height = _height;

	this->WorldSize.x = _width * 1.0 / Scwidth;
	this->WorldSize.y = _height * 1.0 / Scheight;

	this->WorldPos.x = x * 2.0 / Scwidth - 1.0;
	this->WorldPos.y = y * 2.0 / Scheight - 1.0;

}

/*
Decide which part of the UI will be displayed on the screen.
@param[left] : The left border of the painted area. Default: -1
@param[right] : The right border of the painted area. Default: 1
@param[top] : The top border of the painted area. Default: -1
@param[bottom] : The bottom border of the painted area. Default: 1
*/

void UiClass::setPaintArea(float left, float right, float top, float bottom)
{
	float _l = (left + 1.0f) / 2.0f;
	float _r = (right + 1.0f) / 2.0f;
	float _t = (top + 1.0f) / 2.0f;
	float _b = (bottom + 1.0f) / 2.0f;
	Vertex UIquad[] =
	{
		// Positions              // Normals（占位符）   // TexCoords
		{ glm::vec3(right,  bottom, 0) ,glm::vec3(1,1,1), glm::vec2(_r, _t) },
		{ glm::vec3(right , top, 0) ,glm::vec3(0,1,1), glm::vec2(_r, _b) },
		{ glm::vec3(left ,top , 0) ,glm::vec3(1,1,0), glm::vec2(_l, _b) },
		{ glm::vec3(left, bottom, 0) ,glm::vec3(1,1,0), glm::vec2(_l, _t) }
	};
	/*for (int i = 0; i < 4; i++)
	this->vertices[i] = UIquad[i];*/
	this->reMesh(UIquad, 4);
}
