#pragma once

#include "include\GL\glew.h"
#include "include\glm\glm.hpp"
#include "include\glm\gtc\matrix_transform.hpp"
#include "include\glm\gtc\type_ptr.hpp"
#include "Texture.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "shader.h"

struct Particle {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 tone;
	GLfloat zoom;
	GLuint lifespan;
};

typedef std::function<bool(Particle&)> ParticleUpdate; // if Particle is still alive return 1

class ParticleBundle {
public:
	static bool defaultUpdate(Particle& p) {
		if (p.lifespan-- <= 0) { // if a kid never dies out ,lifespan shall not change.
			return false;
		}
		p.position += p.velocity;
		p.velocity.y -= 0.0003f;
		
		return true;
	}
	static bool defaultGenerate(Particle& p) {
		p.lifespan = 600;
		p.position = { (rand() % 200) / 5.0 - 20.0 , 0 , (rand() % 200) / 5.0 - 20.0 };
		p.tone = { 1.0,1.0,1.0,0.9 };
		p.zoom = 1.0;
		p.velocity = { (rand() % 20 / 100.0) , -(rand() % 20 / 200.0) - 0.2,(rand() % 10 / 100.0) };
		return true;
	}
	static bool smogGen(Particle& p) {
		p.lifespan = 80;
		p.position = { rand() % 100 / 100.0 - 0.5f,rand() % 10 / 100.0 + 0.5f,rand() % 100 / 100.0 - 0.5f };
		p.velocity = { rand() % 100 / 100.0, rand() % 100 / 100.0, rand() % 100 / 100.0 };
		p.tone = { 1.0f,0.1f,0.0f,0.0f };
		p.zoom = 0.5;
		return true;
	}
	static bool dropGen(Particle& p) {
		p.lifespan = 0;
		p.position = { 0,0,0 };
		p.velocity = { 0.0,1.0,0.0 };
		p.tone = { 1.0f,1.0f,1.0f,0.8f };
		p.zoom = 0.5;
		return true;
	}
	static bool dropUpd(Particle& p);
	static bool smogUpd(Particle& p) {
		if (p.lifespan-- <= 0) { // if a kid never dies out ,lifespan shall not change.
			p.lifespan = 0;
			p.tone.a = 0;
			return true;
		}
		if (p.lifespan >= 60 && p.lifespan <= 80) {
			p.tone.a -= (p.tone.a - 1.0) / 10.0;
		}
		else {
			p.tone.a = p.lifespan / 40.0;
			if (p.tone.a > 1) p.tone.a = 1.0;
		}
		p.zoom += rand() % 10 / 100.0;
		return true;
	}
	static bool cacheUpd(Particle& p) {
		return true;
	}

	static bool firePUpd(Particle& p) {

		if (p.position.y <= -22) { // if a kid never dies out ,lifespan shall not change.
			
			p.position = { (rand() % 200) / 5.0 - 20.0 , 0 , (rand() % 200) / 5.0 - 20.0 };

		}
		p.position += p.velocity;
		//glm::vec3 anchor = ParticleFactory::Instance().bundles.begin()->anchor;

		/*if (terrain.getHeight(anchor.x + p.position.x, anchor.z + p.position.z) >= p.position.y + anchor.y)
		{
		p.velocity.y = -p.velocity.y;
		}*/

		//p.tone.a = p.lifespan / 40.0;

		return true;

	};
	glm::vec3 anchor;
	ParticleUpdate updateFunc;
	ParticleUpdate generateFunc;
	Texture* texture;
	int width, height;
	bool loop; 
	int size;
	int lifespan;
	int count;
	bool visible;
	std::vector<Particle> kids;
	ParticleBundle(int num=32,bool _loop=false,ParticleUpdate _u = ParticleBundle::defaultUpdate,
		ParticleUpdate _g = ParticleBundle::defaultGenerate);

	void refresh();

	void update();

	void draw(shader& _s);

	const bool end() const {
		return count <= 0 && !loop;
	}
private:
	GLuint VAO, VBO;
};


class ParticleFactory
{
public:
	static ParticleFactory& Instance();
	static shader* _shader;
	std::vector<ParticleBundle> bundles;
	void draw(shader& s=*ParticleFactory::_shader);
	ParticleFactory();
	~ParticleFactory();
};


void addExplosion(glm::vec3 pos);
