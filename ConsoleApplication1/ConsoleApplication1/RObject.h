#pragma once
#include "shader.h"
#include "GameObject.h"
#include <functional>

// interface
// RObject : any object that should be displayed using shader

class RObject : public GameObject
{
public:
	bool visible;
	RObject(GameObject* par=nullptr) : GameObject(par),radiusA(0),radiusB(0) {};
	RObject(const RObject& or,GameObject* par=nullptr) : 
		GameObject(par),
		radiusA(or.radiusA),
		radiusB(or.radiusB)
	{};
	~RObject() {};
	/* 
	Collision test.If radius is set to zero,no collision test will be run.
	radiusA : test bump
	radiusB : test into event zone
	*/
	float radiusA,radiusB;
	std::function<void(RObject&, RObject&)> collisionA, collisionB;
	virtual void draw(shader &_s) = 0;
	virtual void animate(std::string symbol, float & m) {};
	virtual void blend(std::string end, float & m, float per) {  };
	virtual void prepare() {};
	virtual void destroy() {};
	virtual void playAnimation(const std::string& type) {};
	void updateGraphics(shader * Shader) { this->draw(*Shader); }
	/*
	* Collision Test
	* @param[a1] : the point need to nest
	* @return[bool] : is the point in the Cylinder defined by center(position) and size(radius)
	*/
	bool collisionTest(RObject& a1)
	{
		if (radiusA == 0) return false;
		glm::vec3 nowPos = a1.worldPos();
		nowPos.y = position.y;
		if (glm::distance(nowPos, worldPos()) <= radiusA) {
			if (collisionA) collisionA(a1, *this);
			return true;
		} 
		if(glm::distance(nowPos,worldPos()) <= radiusB)
			if(collisionB) collisionB(a1, *this);
		return false;
	}
	/*
	* Collision Test
	* @param[glm::vec3 nowPos] : the point need to nest
	* @param[std::function<void (DObject*)>] : the recall function if a collision is detected.
	* @return[bool] : is the point in the Cylinder defined by center(position) and size(radius)
	*/
	//virtual bool collisionTest(glm::vec3 nowPos, std::function<void(RObject*)> func) = 0;
};

