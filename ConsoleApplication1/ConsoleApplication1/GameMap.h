#pragma once
#include "Light.h"
#include "DObject.h"
#include "SObject.h"
#include "Camera.h"

extern RObject* bump;
extern int hp;

class GameMap
{
public:
	std::string name;
	std::vector<RObject*> render_list;
	std::vector<Light> light_list; // 第一个light永远是天光
	RObject* hero;
	Camera* host;
	GameMap();
	void initialize() {
		std::vector<RObject*>& objects = render_list;
		DObject* ob1 = new DObject("./resources/Jarvan/run.DAE");//new DObject("./resources/bio cha/chris.obj");
		ob1->radiusA = 2.5f;
		ob1->radiusB = 4.0f;
		ob1->registAnimation("./resources/Jarvan/idle.DAE", "idle2");
		//ob1->animate(20);
		DObject* ob2 = new DObject(*ob1);
		DObject* ob = new DObject(*ob1);
		SObject* grasshost = new SObject("./resources/flower.fbx");
		//grasshost->rotationAngles.z = glm::radians(90.0f);
		grasshost->position = { -0.0f,11.0,3.0 };
		grasshost->visible = true;
		grasshost->scale = { 0.05,0.05,0.05 };
		//grasshost->rotationAngles.x = glm::radians(90.0f);
		grasshost->rotationAngles.z = glm::radians(45.0f);
		grasshost->castShadow = false;
		//SObject* ob4 = new SObject("./resources/xrk.obj");
		//SObject* ob5 = new SObject("./resources/xrk.obj");
		SObject* ob3 = new SObject("./resources/bio cha/chris.obj");
		objects.push_back(ob);
		objects.push_back(ob1);
		objects.push_back(ob2);
		//objects.push_back(grasshost);
		//objects.push_back(ob4);
		//objects.push_back(ob5);
		objects.push_back(ob3);
		ob->rotationAngles.z = glm::radians(180.0f);
		ob1->position = glm::vec3(-22.0f, -0.75f, 0.0f);
		ob1->scale = glm::vec3(0.01f, 0.01f, 0.01f);
		ob2->position = glm::vec3(4.0f, -0.75f, 5.0f);
		ob2->scale = glm::vec3(0.01f, 0.01f, 0.01f);
		ob->scale = glm::vec3(0.01f, 0.01f, 0.01f);
		ob3->scale = glm::vec3(0.008f, 0.008f, 0.008f);
		ob3->position = glm::vec3(0.0f, -0.75f, 3.0f);
		ob3->rotationAngles = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);
		/*ob4->visible = true;
		ob4->position = { -4.0f,-0.75f,2.0f };
		ob4->scale = glm::vec3(0.08f, 0.08f, 0.08f);
		ob5->visible = true;
		ob5->position = { -25.0f,-0.75f,3.0f };
		ob5->scale = glm::vec3(0.05f, 0.05f, 0.05f);*/
		ob2->visible = true;
		ob->position = { -1.75f,-0.75f,20.0f };
		ob->visible = true;


		hero = ob;
		hero->forward = { 0.0f,0,-0.6f };

		host = new Camera(hero);
		host->forward = { 0,0,0 };

		ob1->symbol = "LinLin";
		ob2->symbol = "XianZi";

		/*SObject* bd = new SObject("./resources/building.fbx");//new DObject("./resources/bio cha/chris.obj");
		objects.push_back(bd);
		bd->position = { 0,8.0f,0 };
		bd->scale = { 0.02,0.02,0.02 };
		bd->visible = true;*/

		std::function<void(RObject&, RObject&)> tmp = [&](RObject& hero, RObject& tester) {
			bump = &tester;
		};
		std::function<void(RObject&, RObject&)> tmp2 = [&](RObject& hero, RObject& tester) {
			hp -= 0.01;
		};
		ob1->collisionB = (tmp);
		ob2->collisionB = tmp;
		ob1->collisionA = tmp2;
		ob2->collisionA = tmp2;
	};
	void initialize(const char* file) {}; // 从文件初始化地图
	void render(shader& _shader) {};
	void update() {}; // 更新事件
	~GameMap();
};

