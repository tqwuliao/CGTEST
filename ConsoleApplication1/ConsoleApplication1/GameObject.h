#pragma once
#include <vector>
#include "include\GL\glew.h"
#include "include\GLFW\glfw3.h"
#include "include\glm\glm.hpp"

/*
Including:
 - Camera
 - RObject
*/


class GameObject
{
public:
	std::vector<GameObject*> children;
	std::string symbol; // Unique identifier
	GameObject* parent;
	GameObject(GameObject* par=nullptr);
	~GameObject();
	// the center point of the Object
	glm::vec3 position;
	glm::vec3 rotationAngles; // rotationAngles(x,y,z)
	glm::vec3 forward; // forward direction
	glm::mat4 transform(); // transforms
	glm::vec3 scale;
	glm::vec3 worldPos();
	glm::vec3 worldFro();
	void update() {};
private:
	glm::mat4 _transform;
	glm::vec3 _worldPos;
	glm::vec3 _worldFro;
};

