#pragma once
#include "DObject.h"


class GameHero
{
public:
	DObject* model;
	glm::vec3 forward;

	GameHero(DObject* d) : model(d) {
	};

	glm::vec3 stepZero(glm::vec3 ori) {

	}

	void update() {

	}

	void move(glm::vec3 direction) {

	}

	~GameHero();
};

