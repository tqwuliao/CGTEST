#include "SObject.h"
#include "include\glm\glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "CacheManager.h"
#include <iostream>

SObject::SObject(GameObject* par) :
	RObject(par),
	model(NULL)
{
}

SObject::SObject(std::string path, GameObject* par) :
	RObject(par),
	model(NULL)
{
	SModel* t;
	if ((t = CacheManager<SModel>::Instance().getResource(path))) model.reset(t);
	else model.reset(new SModel(path.data()));
}

// 这里的par必须指定，否则会和SObject(GameObject* par)搞混
SObject::SObject(const SObject & data, GameObject* par) :
	RObject(data,par)
{
	model.reset(data.model.get());
}

void SObject::draw(shader & _s)
{
	glm::mat4 model;

	if (!this->model) return;
	if (!visible) return;

	/* relative transform */
	model = transform();
	//std::cout << model;
	_s.bind("model", glm::value_ptr(model));
	this->model->draw(_s);
	//std::cout << model[0].x << model[1].x << model[2].x << std::endl;
}

void SObject::draw(shader & _s,int repeat)
{
	glm::mat4 model;

	if (!this->model) return;
	if (!visible) return;

	/* relative transform */
	model = transform();
	//std::cout << model;
	_s.bind("model", glm::value_ptr(model));
	this->model->draw(_s,repeat);
	//std::cout << model[0].x << model[1].x << model[2].x << std::endl;
}

SObject::~SObject()
{
	model.reset();
}
