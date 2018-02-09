#include "DObject.h"
#include "include\glm\glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "CacheManager.h"
#include <iostream>

glm::vec3 ori;

std::ostream& operator<<(std::ostream& os, glm::vec3 model)
{
	for (int i = 0; i < 3; i++)
	{
		os << model[i] << " ";
	}
	os << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, glm::mat4 model)
{
	for (int i = 0; i < 4; i++)
	{
		for (int n = 0; n < 4; n++)
			os << model[i][n] << " ";
		os << std::endl;
	}
	os << std::endl;
	return os;
}

DObject::DObject(GameObject* par) :
	RObject(par),
	model(NULL)
{
}

DObject::DObject(std::string path, GameObject* par) :
	RObject(par),
	model(NULL)
{
	Model* t;
	if ((t = CacheManager<Model>::Instance().getResource(path))) model.reset(new Model(*t));
	else model.reset(new Model(path.data()));
}

DObject::DObject(const DObject & data, GameObject* par) :
	RObject(data,par)
{
	model.reset(new Model(*data.model));
}


void DObject::animate(std::string symbol,float& tick)
{
	model->animate(symbol, tick);
}

void DObject::blend(std::string end, float & tick, float percent)
{
	model->blend(end, tick, percent);
}

void DObject::draw(shader & _s)
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

void DObject::registAnimation(std::string filepath, std::string symbol)
{
	this->model->registAnimation(filepath, symbol);
}

DObject::~DObject()
{
	model.reset();
}
