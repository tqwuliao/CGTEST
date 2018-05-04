#include "Camera.h"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include <iostream>

extern int Scwidth, Scheight;
extern float yaw,pitch;

extern std::ostream& operator<<(std::ostream& os, glm::mat4 model);

void Camera::bindCamera(shader & _s)
{
	static glm::mat4 view;
	static glm::vec3 pos;
	pos = worldPos();
	static glm::vec3 forward;
	glm::vec3 dir = Front + glm::vec3(0, yaw * 0.5f, 0);
	glm::mat4 pitchrot;
	pitchrot = glm::rotate(pitchrot, pitch / -3.0f, glm::vec3(0, 1.0f, 0));
	glm::vec4 tmp = (pitchrot * glm::vec4(dir, 0.0));
	memcpy(&dir, &tmp, sizeof(glm::vec3));
	forward = pos + dir;
	
	view = glm::mat4(glm::lookAt(pos, forward, Up));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), float(Scwidth*1.0 / Scheight), 0.01f, 100.0f);
	glm::mat4 ortho;
	ortho = glm::ortho(0.0f, (float)Scwidth, (float)0, (float)Scheight);
	_s.bind("view", glm::value_ptr(view));
	_s.bind("projection", glm::value_ptr(projection));
	_s.bind("cameraPos", pos);
	_s.bind("cameraDir", dir);
	_s.bind("viewPos", pos);
	_s.bind("ortho", glm::value_ptr(ortho));
}

Camera::Camera(GameObject* par) :
	GameObject(par),
	fov(45.0f),
	Target({0,0,0}),
	Direction(glm::normalize(Pos - Target)),
	Front(0.f,-1.0f,-5.0f),
	Up(0,1.0f,0)
{
	position = { 0.0f,2.0f,2.5f };
}


Camera::~Camera()
{
}
