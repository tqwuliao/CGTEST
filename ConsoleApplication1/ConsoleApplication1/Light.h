#pragma once
#include "include\glm\glm.hpp"
#include "shader.h"
#include "include\glm\gtc\matrix_transform.hpp"
#include "include\glm\gtc\type_ptr.hpp"

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
	static float max;
	glm::mat4 shadowTransformation;
	void prepareLight(glm::vec3 pos) {
		position = glm::normalize(position);
		glm::mat4 _shadowTransformation;
		glm::mat4 lightOrtho = glm::ortho(-max, max, -max, max, 1.0f, 50.0f);

		_shadowTransformation = lightOrtho * glm::lookAt(
			(-32.0f) * position + pos,
			pos, glm::vec3(0, 1.0f, 0));//glm::vec3(-position.y,position.x+position.z, -position.y));
		this->shadowTransformation = _shadowTransformation;
	}
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
	void bindLight(shader& _shader, std::string entry,int index) {
		
		_shader.bind(entry, glm::value_ptr(shadowTransformation),index);
	}
	void bindLight(shader& _shader) {
		position = glm::normalize(position);
		std::stringstream sss;
		sss << "dirLight.";
		std::string prefix = sss.str();
		_shader.bind("dirLightPos", position);
		_shader.bind(prefix + "ambient", ambient);
		_shader.bind(prefix + "diffuse", diffuse);
		_shader.bind(prefix + "specular", specular);
		_shader.bind(prefix + "direction", position);
		_shader.bind(prefix + "enable", 1);
		
		_shader.bindF(prefix + "far", 50.0f);
		
		_shader.bind(prefix + "transform", glm::value_ptr(shadowTransformation));
	}
};

