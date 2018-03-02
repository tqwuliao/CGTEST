#pragma once
#include "SMesh.h"

class Terrain
{
public:
	static int TerrainSize;
	SVertex * generateWaterFace(SVertex * ori,const char* data);
	Terrain();
	void updateGraphics(shader * shader) { waterFace->Draw(*shader); };
	glm::vec3 getNormal(float x,float y) {
		int PosY = x*10 + 512;
		int PosX = y*10 + 512;
		float frac = x*10 + 512 - PosY;
		int pos = PosY * 1024 + PosX;
		glm::vec3 _yA = waterfloor[pos].Normal * (1 - frac) + waterfloor[pos + 1024].Normal * (frac);
		glm::vec3 _yB = waterfloor[pos+1].Normal * (1 - frac) + waterfloor[pos + 1025].Normal * (frac);
		frac = y * 10 + 512 - PosX;
		//hero->position.y = -2.75 + _yA * (1-frac) + _yB * frac;
		return (_yA * (1 - frac) + _yB * frac);
	}
	float getHeight(float x, float y) {
		int PosY = x * 10 + 512;
		int PosX = y * 10 + 512;
		float frac = x * 10 + 512 - PosY;
		int pos = PosY * 1024 + PosX;
		
		float _yA = cache[pos].Position.y * (1 - frac) + cache[pos + 1024].Position.y * (frac);
		float _yB = cache[pos+1].Position.y * (1 - frac) + cache[pos + 1025].Position.y * (frac);
		frac = y * 10 + 512 - PosX;
		//hero->position.y = -2.75 + _yA * (1-frac) + _yB * frac;
		return _yA * (1 - frac) + _yB * frac - 2.75; // 20.0这里是scale大小, -2.75有Translation,这里可以加参数
	}
	~Terrain();
private:
	unsigned char* heightMap;
	SVertex *waterfloor;
	GLuint *waterindex;
	SMesh *waterFace;
	SVertex* cache;
};

