#pragma once
#include "Mesh.h"

typedef struct _RECT{
	int x, y, width, height;
} Rect;

class UiClass : public Mesh
{
public:
	UiClass();
	UiClass(std::string bgName);
	~UiClass();
	Texture background;
	glm::vec2 WorldPos,WorldSize;
	float opacity;
	Rect Geometry;
	void Draw(shader& Shader);
	//int width, height;
	void setGeometry(int x, int y);
	void setGeometry(int x, int y, int width, int height);
	/*
	Decide which part of the UI will be displayed on the screen.
	@param[left] : The left border of the painted area. Default: -1
	@param[right] : The right border of the painted area. Default: 1
	@param[top] : The top border of the painted area. Default: -1
	@param[bottom] : The bottom border of the painted area. Default: 1
	*/
	void setPaintArea(float left, float right, float top, float bottom);
};

