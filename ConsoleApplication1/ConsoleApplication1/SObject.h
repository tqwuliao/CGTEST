#pragma once
#include "SModel.h"
#include "RObject.h"
#include "GameObject.h"
#include <functional>
#include <memory>

class SObject : public RObject
{
public:

	// create an empty DObject.If model==NULL or !visible,will not be drawn
	SObject(GameObject* par = nullptr);
	// create an DObject with a given model
	SObject(std::string path, GameObject* par = nullptr);
	// create an DObject shared the same Model
	SObject(const SObject&, GameObject* par = nullptr);
	/* Draw Object using a specified shader
	the shader must have a model specified to be a unique
	*/
	void draw(shader& _s);
	/*
	* Collision Test
	* @param[glm::vec3 nowPos] : the point need to test
	* @return[bool] : is the point in the Cylinder defined by center(position) and size(radius)
	*/
	//virtual bool collisionTest(glm::vec3 nowPos);
	/*
	* Collision Test
	* @param[glm::vec3 nowPos] : the point need to test
	* @param[std::function<void (DObject*)>] : the recall function if a collision is detected.
	* @return[bool] : is the point in the Cylinder defined by center(position) and size(radius)
	*/
	//virtual bool collisionTest(glm::vec3 nowPos, std::function<void(RObject*)> func);
	~SObject();
private:
	std::shared_ptr<SModel> model;

	SObject& operator=(const SObject&) {};
};

