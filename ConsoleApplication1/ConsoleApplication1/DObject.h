#pragma once
#include "Model.h"
#include "RObject.h"
#include "GameObject.h"
#include <functional>
#include <memory>

class DObject : public RObject
{
public:
	
	// create an empty DObject.If model==NULL or !visible,will not be drawn
	DObject(GameObject* par=nullptr);
	// create an DObject with a given model
	DObject(std::string path, GameObject* par = nullptr);
	// create an DObject shared the same Model
	DObject(const DObject&, GameObject* par = nullptr);
	
	virtual void playAnimation(const std::string& str) { animate(str, animateCount); animateCount++; };
	void animate(std::string symbol,float& tick);
	void blend(std::string end, float& tick, float percent);
	/* Draw Object using a specified shader
	the shader must have a model specified to be a unique
	*/
	void draw(shader& _s);
	void draw(shader & _s, int repeat);
	void registAnimation(std::string filepath, std::string symbol);
	void prepare() { model->prepare(); };
	/*
	* Collision Test
	* @param[glm::vec3 nowPos] : the point need to nest
	* @return[bool] : is the point in the Cylinder defined by center(position) and size(radius)
	*/
	//virtual bool collisionTest(glm::vec3 nowPos);
	/*
	* Collision Test
	* @param[glm::vec3 nowPos] : the point need to nest
	* @param[std::function<void (DObject*)>] : the recall function if a collision is detected.
	* @return[bool] : is the point in the Cylinder defined by center(position) and size(radius)
	*/
	//virtual bool collisionTest(glm::vec3 nowPos, std::function<void(RObject*)> func);
	std::shared_ptr<Model> model;
	~DObject();
private:
	
	float animateCount = 0;
	DObject& operator=(const DObject&) {};
};

