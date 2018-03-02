#pragma once
#include "include\glm\glm.hpp"
#include "shader.h"

class Light
{
public:
	glm::vec3 ambient,diffuse,specular,position;
	float constant, linear, quadratic;
	Light();
	~Light();
	
	virtual void bindLight(shader& _shader, int pos) = 0;
};

class PointLight : public Light {
public:
	void bindLight(shader& _shader, int pos) {
		std::stringstream sss;
		sss << "pointLight[";
		sss << pos;
		sss << "].";
		std::string prefix = sss.str();
		_shader.bind(prefix + "ambient", ambient);
		_shader.bind(prefix + "diffuse", diffuse);
		_shader.bind(prefix + "specular", specular);
		_shader.bind(prefix + "position", position);
		_shader.bindF(prefix + "constant", 1.0f);
		_shader.bindF(prefix + "linear", linear);
		_shader.bindF(prefix + "quadratic", quadratic);
		_shader.bind(prefix + "enable", 1);
	}
};

class DirLight : public Light {
public:
	void bindLight(shader& _shader, int pos) {
		std::stringstream sss;
		sss << "dirLight.";
		std::string prefix = sss.str();
		_shader.bind(prefix + "ambient", ambient);
		_shader.bind(prefix + "diffuse", diffuse);
		_shader.bind(prefix + "specular", specular);
		_shader.bind(prefix + "direction", position);
		_shader.bind(prefix + "enable", 1);
	}
};