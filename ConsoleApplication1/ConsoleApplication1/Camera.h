#pragma once
#include "GameObject.h"
#include "shader.h"

class Camera : public GameObject
{
public:
	GLfloat fov;
	glm::vec3 Pos;
	glm::vec3 Target;
	glm::vec3 Direction;
	glm::vec3 Front;
	glm::vec3 Up;
	void bindCamera(shader& _s);
	glm::vec3 cPos();
	Camera(GameObject* par=nullptr) ;
	~Camera();
};

