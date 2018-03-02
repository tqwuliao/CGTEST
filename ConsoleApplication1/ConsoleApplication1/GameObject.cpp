#include "GameObject.h"
#include "include/glm/gtc/matrix_transform.hpp"

GameObject::GameObject(GameObject * par) : 
	parent(par),
	position({ 0,0,0 }),
	scale({ 0,0,0 }),
	rotationAngles({ 0,0,0 })
{
}

GameObject::~GameObject()
{
}

glm::mat4 GameObject::transform()
{
	static glm::vec3 pos = {0,0,0}
	, scl = { 0,0,0 }, rotV = { 0,0,0 };
	static glm::vec3 xAxis = { 1,0,0 },
		yAxis = { 0,1,0 },
		zAxis = { 0,0,1 };
	static float halfPI = glm::radians(90.0f);
	if (pos != position || scl != scale || rotationAngles != rotV)
	{
		pos = position;
		scl = scale;
		rotV = rotationAngles;
		_transform = glm::mat4();
		_transform = glm::translate(_transform, position);
		_transform = glm::rotate(_transform, rotV.x - halfPI, xAxis);// 立起来！
		_transform = glm::rotate(_transform, rotV.z - halfPI * 0.0f, zAxis); 
		_transform = glm::rotate(_transform, rotV.y, yAxis);
		_transform = glm::scale(_transform, scl);
	}
	if(parent) _transform = parent->transform() * _transform;
	return _transform;
}

glm::vec3 GameObject::worldPos()
{
	static glm::vec3 pos;
	if (pos != position) {
		pos = position;
		_worldPos = pos;
	}
	if (parent) return _worldPos + parent->worldPos();
	return _worldPos;
}

glm::vec3 GameObject::worldFro()
{
	static glm::vec3 pos;
	if (pos != forward) {
		pos = forward;
		_worldFro = pos;
	}
	if (parent) return glm::normalize(_worldFro + parent->worldFro());
	return glm::normalize(_worldFro);
}
