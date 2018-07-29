
#include "include\GL\glew.h"
#define GLEW_STATIC
#include "include\GLFW\glfw3.h"
#include "SOIL.h"
#include <string>
#include <fstream>
#include <iostream>
#include <thread>

#define GLUT_DISABLE_ATEXIT_HACK
#pragma warning(disable : 4305)
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"SOIL.lib")
#pragma comment(lib,"assimp-vc140-mt.lib")
#include "shader.h"
#include "include\glm\glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "UiClass.h"
#include "DObject.h"
#include "SObject.h"
#include "Camera.h"
#include "Light.h"
#include "TextRenderer.h"
#include "ParticleFactory.h"
#include "Terrain.h"
#include <mutex>

#include "CacheManager.h"

float hp = 2.0f;

struct AnimationWork {
	bool blend;
	RObject* obj;
	std::string symbol;
	float* time;
	float percent;
} Animations[100];

extern int Scwidth, Scheight;
Camera * host; // the camera that capture the screen;
DObject * hero; // the character we will be manipulate;
GameObject * bump; // the object the actor bump into.
UiClass* mouse;
bool running;
float runRemain;
glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
GLfloat lastX = 400, lastY = 300;
GLfloat yaw = 0.0f, pitch = 0.0f;
glm::vec3 lightPos = glm::vec3(0.5f, 23.0f, 4.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat backForce = 0.0f; //后坐力。。。
GLfloat upForce = 0.0f; // jump
bool hidden = false; //hidden??
glm::mat4 Transform_2D; // 从pixel到float
glm::vec2 mousePos;
glm::vec2 changePos = { 25,25 }; // 画地形的pos
float changeScl = 0.01f; // 画地形的强度
GLfloat lightArc = 0.0f;
std::vector<std::thread*> tasks(10);
std::recursive_mutex lockA, lockB;
int doingOne, finishNum, workNum;

GLfloat quad[] = {
	// Positions   // TexCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
}; // plane

GLfloat weights[6][6] = {
	0,1,1,1,1,0,
	1,2,3,3,2,1,
	3,5,5,5,5,3,
	3,5,5,5,5,3,
	1,2,3,3,2,1,
	0,1,1,1,1,0
};

SVertex *waterfloor;
GLuint *waterindex;
SMesh *waterFace;

GLfloat skyboxVertices[] = {
            // Back face
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
			-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
            // Bottom face
            -1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
        };

extern GLuint loadCubemap(std::vector<const GLchar*> faces);

std::vector<const GLchar*> faces;
std::vector<RObject*> objects;
unsigned char* heightMap;

SVertex* generateWaterFace(SVertex* ori)
{
	if (ori == NULL)
	{
		const int size = 1024;
		heightMap = Texture::loadHeightMap(".\\resources\\heightMap.png");
		ori = new SVertex[size*size];
		for (int i = 0;i < size;i++)
		{
			for (int j = 0;j < size;j++)
			{
				float add = (rand() % 100) / 200.0;
				ori[i * size + j].Position = { (i) - (size / 2),
					heightMap[(i*size+j)*3]/20.0,(j) - (size / 2) };
				ori[i * size + j].Normal = { 0,1,0 };
				ori[i * size + j].TexCoords = { i / 8.0 + add / 2.0,j / 8.0 + add / 10.0};
			}
		}
		
		waterindex = new GLuint[(size-1)*(size-1)*6];
		
		for (int i = 0;i < size-1;i++)
		{
			for (int j = 0;j < size-1;j++)
			{
				waterindex[i * (size - 1) * 6 + j * 6] = i * size + j;
				waterindex[i * (size - 1) * 6 + j * 6 + 1] = i * size + j + 1;
				waterindex[i * (size - 1) * 6 + j * 6 + 2] = i * size + j + size;
				waterindex[i * (size - 1) * 6 + j * 6 + 3] = i * size + j + size;
				waterindex[i * (size - 1) * 6 + j * 6 + 4] = i * size + j + 1;
				waterindex[i * (size - 1) * 6 + j * 6 + 5] = i * size + j + size + 1;
			}
		}
		return ori;
	}
	return ori;
}

glm::vec3 operator*(glm::vec3 a, float b)
{
	glm::vec3 c = a;
	c *= b;
	return c;
}

void animate_thread(RObject* obj, const char* name, float& tick)
{
	//double now_time = glfwGetTime();
	obj->animate(name, tick);
	//printf("%lf\n", glfwGetTime() - now_time);
}

void blend_thread(RObject* obj, const char* name, float& tick, float percent)
{
	obj->blend(name, tick, percent);
}

void animate_load() 
{
	int index;
	while (1)
	{
		if (doingOne < workNum)
		{
			lockA.lock(); // lock doingOne,match the one that has already been worked on
			if (doingOne >= workNum) continue; // 如果在得到锁之前有其它线程获得了该工作
			index = doingOne++;
			lockA.unlock();
			AnimationWork tmp = Animations[index];
			if (tmp.blend) blend_thread(tmp.obj, tmp.symbol.data(), *tmp.time, tmp.percent);
			else animate_thread(tmp.obj, tmp.symbol.data(), *tmp.time);
			lockB.lock(); // lock FinishNum
			finishNum++;
			lockB.unlock();
		}
	}
}



void gravity() {
	float anchor = hidden ? 1.25f : 3.25f;
	const float a = 0.02f;
	/*static float v = 0;
	float v_ = upForce - v;*/
	/*
	if (host->cPos().y + upForce < anchor) { 
		if (upForce > 0) {
			host->cPos().y += upForce;
			upForce -= a;
		} // from hidden
		else {
			cameraPos.y = anchor; upForce = 0;
		}
	}
	else
	{
		cameraPos.y += upForce;
		upForce -= a;
	}*/
}
extern std::vector<glm::vec3> drops;
extern int dropsize;
extern ParticleUpdate testfunc;
SObject* grasshost;
Terrain* terrain;

void render()
{
	static shader * _shader,*_shader2,*screenShader,*skyboxshader,*shadowshader,*bloomshader,*tDshader,*wshader;
	static shader *staticShader,*pShader,*shadowshader1;
	//static Texture texture1(".\\resources\\container2.png");
	//static Texture texture2(".\\resources\\container2_specular.png");
	static GLuint FBO = 0,shadowFBO,shadowMap,shadowMap2,blurFBO[2];
	static GLuint screen[3],rbo,blurMap[4];
	static Texture floorTex("./resources/203668.jpg");
	static Texture waterTex("./resources/waternormal.png");
	static Texture waterTex2("./resources/waternormal2.png");
	static Texture bokeh("./resources/bokeh2.jpg");
	static UiClass mousePic(".\\resources\\mouse.png");
	static glm::mat4 testArr[100];
	static PointLight pointLight0;
	static DirLight dirLight0;
	static GLuint buffer;
	static GLuint quadArr = -1, quadVBO, skyboxVAO, skyboxVBO;
	
	gravity();
	GLfloat pitch = ::pitch + ::backForce;
	if (backForce > 0.2f) // 无视 这是模拟持枪后坐力的=.=
	{
		backForce /= 1.15f;
		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		if(hero) hero->forward = glm::normalize(front);
		if (backForce <= 0.02f) backForce = 0.0f;
		
	}
	
	if (!_shader) // 初始化 可以后面单独提出一个方法
	{
		terrain = new Terrain();
		_shader = new shader(".\\GLSL\\vertex.hlsl", ".\\GLSL\\color.hlsl");
		//_shader2 = new shader(".\\GLSL\\vertex.g.txt", ".\\GLSL\\light.g.txt");
		screenShader = new shader(".\\GLSL\\vertex2.g.txt", ".\\GLSL\\scene.hlsl");
		skyboxshader = new shader(".\\GLSL\\skyver.g.txt", ".\\GLSL\\skysha.g.txt");
		shadowshader = new shader(".\\GLSL\\shadow.vs", ".\\GLSL\\shadow.fs", ".\\GLSL\\shadow.gs");
		shadowshader1 = new shader(".\\GLSL\\shadow.1.vs", ".\\GLSL\\shadow.fs", ".\\GLSL\\shadow.gs");
		bloomshader = new shader(".\\GLSL\\vertex2.g.txt", ".\\GLSL\\blur.hlsl");
		tDshader = new shader(".\\GLSL\\ui.g.txt", ".\\GLSL\\ui.f.txt");
		wshader = new shader(".\\GLSL\\watersky.v.txt", ".\\GLSL\\watersky.g.txt");
		pShader = new shader(".\\GLSL\\plant.g.txt", ".\\GLSL\\color.hlsl");
		//staticShader = new shader(".\\GLSL\\static.g.txt", ".\\GLSL\\color.g.txt");
		ParticleFactory::_shader = new shader("./GLSL/pf.v.txt", "./GLSL/pf.f.txt", "./GLSL/pf.g.txt");
		faces.push_back(".\\resources\\skybox\\right.jpg");
		faces.push_back(".\\resources\\skybox\\left.jpg");
		faces.push_back(".\\resources\\skybox\\top.jpg");
		faces.push_back(".\\resources\\skybox\\bottom.jpg");
		faces.push_back(".\\resources\\skybox\\back.jpg");
		faces.push_back(".\\resources\\skybox\\front.jpg");

		
		DObject* ob1 = new DObject("./resources/Jarvan/run.DAE");//new DObject("./resources/bio cha/chris.obj");
		ob1->radiusA = 2.5f;
		ob1->radiusB = 4.0f;
		ob1->registAnimation("./resources/Jarvan/idle.DAE", "idle2");
		//ob1->animate(20);
		DObject* ob2 = new DObject(*ob1);
		DObject* ob = new DObject(*ob1);
		grasshost = new SObject("./resources/flower.fbx");
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
		/*waterfloor = generateWaterFace(waterfloor);
		waterFace = new SMesh(waterfloor, 1024 * 1024, waterindex, 1023 * 1023 * 6);
		waterFace->setup();*/
		std::cout << "FLoor Id " << floorTex.id() << std::endl;

		

		mouse = &mousePic;

		

	}
	//static DObject& ob = *objects[0];
	//float angle = glm::dot(cameraFront, cameraUp) / glm::length(cameraFront) / glm::length(cameraUp);
	//ob.position = cameraPos + glm::normalize(cameraUp - cameraFront*angle) * ((-3.6f)*1.1f) + cameraFront * 1.5;
	
	static GLuint skybox = loadCubemap(faces);
	/* initialize */ if (FBO == 0)
	{
		glGenFramebuffers(1, &FBO); // 帧缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
		glGenTextures(3, screen);
		for (int i = 0;i < 3;i++) // 生成三个纹理 原颜色，深度，亮光
		{
			glBindTexture(GL_TEXTURE_2D, screen[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
				Scwidth, Scheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, screen[i], 0);
			
		}
		
		glGenRenderbuffers(1, &rbo); // 渲染缓冲 我也不知道干嘛的 教程就这么用的 貌似是深度缓冲
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Scwidth, Scheight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3, attachments); // 绑定三个out
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "FrameBuffer Created." << std::endl;
		}
		else
			std::cout << "Fail to create a FrameBuffer." << std::endl;
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // 释放
		
		glGenFramebuffers(1, &shadowFBO); // 新的帧缓冲，阴影
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glGenTextures(1, &shadowMap); // 生成一张盒形阴影贴图
		glGenTextures(1, &shadowMap2); // 生成一张盒形阴影贴图
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
		for (GLuint i = 0; i < 6; ++i) // 6个面分别设置
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0); // 绑定为深度缓存的记录
		
		glBindTexture(GL_TEXTURE_2D, shadowMap2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap2, 0); // 绑定为深度缓存的记录

		glDrawBuffer(GL_NONE); // 无输出
		glReadBuffer(GL_NONE); 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenFramebuffers(2, blurFBO); // 生成两个帧缓冲为啥两个我忘了 用于景深/BLOOM
		GLuint attachmentsA[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
		};
		for (int n = 0;n < 2;n++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[n]);
			glGenTextures(2, &blurMap[n*2]);
			for (int i = 0;i < 2;i++)
			{
				glBindTexture(GL_TEXTURE_2D, blurMap[i+n*2]);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA16F, Scwidth, Scheight, 0, GL_RGBA, GL_FLOAT, NULL
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, blurMap[i+n*2], 0
				);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
				
			}
			
			glDrawBuffers(2, attachmentsA);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		
		testfunc = [&](Particle& p) {
			float y = terrain->getHeight(p.position.x, p.position.z);
			y = y < 7.25 ? 7.25 : y;
			if (y >= p.position.y + 18) { // if a kid never dies out ,lifespan shall not change.
				p.position = { (rand() % 200) / 5.0 - 20.0 , 0 , (rand() % 200) / 5.0 - 20.0 };
				if (y != 7.25) return true;
				if (dropsize >= 100) return true;
				drops[dropsize] = { p.position.x, 7.27, p.position.z };
				dropsize++;
				return true;
			}
			p.position += p.velocity;
			return true;

		};

		for (int i = 0;i < 100;i++) {
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(i / 20.0, -rand() % 100 / 2000.0 - 2.0f, i % 20));
			//model = glm::scale(model, glm::vec3(rand() % 100 / 200.0 + 0.4 , rand() % 100 / 200.0 + 0.4, rand() % 100 / 200.0 + 0.4));
			model = glm::rotate(model, (float)(rand() % 100 / 33.0), glm::vec3(0, 1, 0));
			
			testArr[i] = model;
		}

		
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::mat4), testArr, GL_STATIC_DRAW);
		for each (auto& var in (static_cast<SObject*>(grasshost))->model->meshes)
		{
			glBindVertexArray(var->VAO);
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(7);
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);
			glVertexAttribDivisor(7, 1);
			glVertexAttribDivisor(8, 1);

			glBindVertexArray(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (quadArr == -1) // 初始化 可以 移到 新方法里。QUAD无视，SKYBOX是天空盒子
		{
			glGenVertexArrays(1, &quadArr);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadArr);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
			glBindVertexArray(0);

			glGenVertexArrays(1, &skyboxVAO);
			glGenBuffers(1, &skyboxVBO);
			glBindVertexArray(skyboxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glBindVertexArray(0);
		}
	}

	
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	static float AnimateTick = 0;
	static int runTick = 0;
	
	double time = glfwGetTime();

	static float an2 = 0;
	
	if (runTick > 1) // animation test.Can ignore
	{
		int index = 0;
		while (finishNum < workNum);
		for (int i = 0;i < workNum;i++)
			Animations[i].obj->prepare();
		finishNum = workNum = doingOne = 0;
		glm::vec3 pos1 = objects[1]->worldPos();
		pos1 -= hero->worldPos();
		pos1.y = 0;
		runTick = 0.0f;
		an2 += 1.0f;
		AnimateTick += 1.0f;
		if (glm::length(pos1) <= 20.0f)
		{	//objects[1]->animate("idle2", AnimateTick);
			animate_thread(objects[1], "idle2", std::ref(AnimateTick));
			//Animations[workNum++] = AnimationWork{false,objects[1],"idle2",&AnimateTick,0};
			//tasks[index++] = (new std::thread(animate_thread, objects[1], "idle2", std::ref(AnimateTick)));
			
		}
		pos1 = objects[2]->worldPos();
		pos1 -= hero->worldPos();
		pos1.y = 0;
		if (glm::length(pos1) <= 20.0f)
		{
			animate_thread(objects[2], "idle2", std::ref(AnimateTick));
			//Animations[workNum++] = AnimationWork{ false,objects[2],"idle2",&AnimateTick,0 };
		} 	
		if (running)
		{
			//if (runRemain == 0) runRemain = 0.5;
			if (runRemain < 1.0)
			{
				//Animations[workNum++] = AnimationWork{ true, hero,"default",&an2,runRemain };
				//tasks[index++] = (new std::thread(blend_thread, hero, "default", std::ref(an2), runRemain));
				hero->blend("default", an2, runRemain);
				runRemain += (1.0 / 8.0);
				if (runRemain >= 1) an2 = 0;
			}
			else
			{
				//Animations[workNum++] = AnimationWork{ false, hero,"default",&an2,0 };
				//tasks[index++] = (new std::thread(animate_thread, hero, "default", std::ref(an2)));
				hero->animate("default", an2);
			}
		}
		else {
			//if (runRemain >= 1.0) runRemain = 0.5;
			if (runRemain > 0.0)
			{
				//Animations[workNum++] = AnimationWork{ true, hero,"idle2",&an2,1-runRemain };
				//tasks[index++] = (new std::thread(blend_thread, hero, "idle2", std::ref(an2), 1-runRemain));
				hero->blend("idle2", an2, 1 - runRemain);
				runRemain -= (1.0 / 8.0);
				if (runRemain <= 0) an2 = 0;
			}
			else
			{
				//Animations[workNum++] = AnimationWork{ false, hero,"idle2",&an2,0 };
				//tasks[index++] = (new std::thread(animate_thread, hero, "idle2", std::ref(an2)));
				hero->animate("idle2", an2);
			}
		}
		runTick++;
	}
	// 画阴影。其实第一部分是设置光源、以及设置物体的位置，其实这部分可以摆在shader之前
	shadowshader->run();
	glViewport(0, 0, 4096,4096);
	GLfloat near_plane = 1.0f, far_plane = 25.0f;
	//glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f),1.0f, near_plane, far_plane);
	glm::mat4 lightOrtho = glm::ortho(-24.0f, 24.0f, -24.0f, 24.0f, near_plane, far_plane);
	glm::mat4 model;
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(lightProjection *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	glm::mat4 shadowTransformation;
	shadowTransformation = lightOrtho * glm::lookAt(
		-32.0f * glm::vec3(0.2f,-0.53f, -0.1f)+hero->position, 
		16.0f * glm::vec3(0.2f, -0.53f, -0.1f) + hero->position,glm::vec3(0,1.0f,0));
	glm::mat4 model3;
	
	
	model3 = glm::translate(model3, glm::vec3(0.0f, 8.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model3 = glm::scale(model3, glm::vec3(1.0f, 1.0f, 1.0f));	// It's a bit too big for our scene, so scale it down
	model3 = glm::rotate(model3, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//
	
    // 这是地形的变换矩阵！！
	glm::mat4 model4;
	model4 = glm::translate(model4, glm::vec3(0.0f, -2.75f, 0.0f)); 
	model4 = glm::scale(model4, glm::vec3(1.0f, 1.0f, 1.0f));

	glm::mat4 oneMat;

	dirLight0.ambient = glm::vec3{ 0.05f,0.05f,0.05f };
	dirLight0.specular = glm::vec3{ 0.6f,0.4f,0.4f };
	dirLight0.diffuse = glm::vec3{ 0.9f,0.9f,0.9f };
	//GLfloat time = glfwGetTime() / 100.0;
	dirLight0.position = glm::vec3{ glm::sin(time / 2.0)/2.0+1.0, -0.46f , 0.3f };
	dirLight0.prepareLight(hero->position + 28.0f*hero->forward);
	dirLight0.bindLight(*shadowshader, "shadowMatrices", 6);
	//shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransformation),6);
	shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransforms[0]), 0);
	shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransforms[1]),1);
	shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransforms[2]),2);
	shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransforms[3]),3);
	shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransforms[4]),4);
	shadowshader->bind("shadowMatrices", glm::value_ptr(shadowTransforms[5]),5);
	//shadowshader->bind("DirLight", 1);
	shadowshader->bind("lightPos", lightPos);
	lightPos.x = 3*glm::sin(glfwGetTime() / 10.0); // 动态光源233
	lightPos.z = 3*glm::cos(glfwGetTime() / 10.0);
	//shadowshader->bindF("far_plane", 49.0);
	glCullFace(GL_BACK);

	runTick += 1.0f;

	for (int i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->castShadow) continue;
		glm::vec3 pos1 = objects[i]->worldPos();
		pos1 -= hero->worldPos();
		pos1.y = 0;
		if (glm::length(pos1) <= 20.0f) // 限定绘制范围，提高帧率（其实没啥用
		{
			objects[i]->prepare(); // 这一步是刷骨骼缓冲
			objects[i]->draw(*shadowshader);
		}
	}

	shadowshader->bind("model", glm::value_ptr(model4));
	terrain->updateGraphics(shadowshader); // 画地形的阴影

	
	shadowshader1->use();
	dirLight0.bindLight(*shadowshader1, "shadowMatrices", 6);
	//shadowshader1->bindF("far_plane", 49.0);
	grasshost->draw(*shadowshader1, 100);
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, Scwidth, Scheight);
	

	glCullFace(GL_BACK); // don't forget to reset original culling face
	//int pos = ((hero->position.x + 512) * 1024 + hero->position.z + 512);

	for (int i = 0;i < objects.size();i++)
	{
		objects[i]->position.y = terrain->getHeight(objects[i]->position.x, objects[i]->position.z); // 位置检测
	}

	int PosY = hero->position.x + 512;
	int PosX = hero->position.z + 512;

	
	/*lightDisplay*/{
		const glm::vec3 ambient = { 0.18,0.18,0.18 };
		const glm::vec3 diffuse = { 0.8,0.8,0.62 };
		const glm::vec3 specular = { 0.8,0.8,0.65 };
		lightArc = 2;
		pointLight0.ambient = ambient * ((lightArc - 1)*(lightArc - 1));
		pointLight0.diffuse = diffuse * ((lightArc - 1)*(lightArc - 1));
		pointLight0.specular = specular * ((lightArc - 1)*(lightArc - 1));
		pointLight0.position = lightPos;
		pointLight0.constant = 1.0;
		pointLight0.linear = 0.09;
		pointLight0.quadratic = 0.0125;
	};
	 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);
	glDepthMask(GL_FALSE);
	skyboxshader->run();
	host->bindCamera(*skyboxshader); // 为主角设置一个相机
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	skyboxshader->bind("skybox", 8);
	glDrawArrays(GL_TRIANGLES, 0, 36); // 画天空盒
	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	_shader->run();


	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadowMap2);
	_shader->bindF("far_plane",25.0);
	_shader->bind("shadowMap2", 8);
	_shader->bind("shadowMap", 6);
	
	_shader->bind("offsets[0]", glm::value_ptr(oneMat));
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, waterTex.id());
	_shader->bind("waterMap", 9);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, waterTex2.id());
	_shader->bind("waterMap2", 10);
	_shader->bindF("time", glfwGetTime() / 2.0);

	

	host->bindCamera(*_shader);


	
	pointLight0.bindLight(*_shader, 0);
	dirLight0.bindLight(*_shader);

	_shader->bindF("time", glfwGetTime());

	//_shader->bind("dirLight.transform", glm::value_ptr(shadowTransformation));
	for (int i = 0; i < objects.size(); i++)
	{
		glm::vec3 pos1 = objects[i]->worldPos();
		pos1 -= hero->worldPos();
		pos1.y = 0;
		if (glm::length(pos1) <= 20.0f)
			objects[i]->draw(*_shader);
	}

	_shader->bind("model", glm::value_ptr(model4));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTex.id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	_shader->bind("material.texture_diffuse1", 0);
	_shader->bind("material.texture_specular1", 0);
	terrain->updateGraphics(_shader);
	glBindTexture(GL_TEXTURE_2D, 0);

	pShader->run();
	//pointLight0.bindLight(*pShader, 0);
	dirLight0.bindLight(*pShader, 0);
	pShader->bind("dirLight.transform", glm::value_ptr(shadowTransformation));
	pShader->bindF("time", glfwGetTime());
	pShader->bindF("near_plane", 1.0);
	pShader->bindF("far_plane", 25.0);
	pShader->bind("shadowMap", 6);
	pShader->bind("shadowMap2", 8);
	host->bindCamera(*pShader);
	grasshost->draw(*pShader, 100);

	glDisable(GL_CULL_FACE);
	
	wshader->run();
	dirLight0.bindLight(*wshader);
	wshader->bind("model", glm::value_ptr(model3));
	host->bindCamera(*wshader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	wshader->bind("skybox", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterTex.id());
	wshader->bind("waterMap", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterTex2.id());
	wshader->bind("waterMap2", 2);
	wshader->bindF("time", glfwGetTime()/2.0);

	terrain->updateGraphics(wshader);
	/*glBindVertexArray(quadArr);
	
	glDrawArrays(GL_TRIANGLES, 0, 6); // 画天空盒
	glBindVertexArray(0);*/
	glEnable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);

	glBlendFunc(GL_ONE, GL_ONE);
	ParticleFactory::_shader->run();
	host->bindCamera(*ParticleFactory::_shader);
	ParticleFactory::Instance().draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
#ifndef BLOOMTEST
	bloomshader->run();
	for (int i = 0;i < 20;i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[(i+1) % 2]);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		if (i == 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(
				GL_TEXTURE_2D, screen[1]
			);
			bloomshader->bind("image", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(
				GL_TEXTURE_2D, screen[0]
			);
			bloomshader->bind("near", 1);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(
				GL_TEXTURE_2D, blurMap[0+(i%2)*2]
			);
			bloomshader->bind("image", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(
				GL_TEXTURE_2D, blurMap[1+(i % 2) * 2]
			);
			bloomshader->bind("near", 1);
		}
		
		
		bloomshader->bind("horizen", i % 2);
		glBindVertexArray(quadArr);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_NONE);
	
	screenShader->run();
	host->bindCamera(*screenShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen[0]);
	screenShader->bind("screenTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,  blurMap[0]);
	screenShader->bind("bloomBlur", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blurMap[1]);
	screenShader->bind("near", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, screen[2]);
	screenShader->bind("far", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bokeh.id());
	screenShader->bind("bokeh",4);
	glBindVertexArray(quadArr);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glm::mat4 textTry;
	char tmp[100];
	textTry = glm::translate(textTry, glm::vec3{ 20.0f,400.0f,0 });
	sprintf(tmp, "Pos %d,%d,%3.0f", PosX,PosY,hero->position.y);
	TextRenderer::Instance().draw(tmp, glm::vec3{ 0.8f,0.9f,0.2f }, textTry);
	textTry = glm::translate(textTry, glm::vec3{ 0.0f,50.0f,0 });
	textTry = glm::scale(textTry, glm::vec3{ 1.1f,0.8f,0 });
	
	sprintf(tmp, "SCALE \\c[1]%d\\c[0] HAHA", (int)(changeScl*1000));
	TextRenderer::Instance().draw(tmp, glm::vec3{ 0.8f,0.9f,0.2f }, textTry);
	textTry = glm::translate(textTry, glm::vec3(0, -120.0f, 0));
	if (bump) {
		textTry = glm::scale(textTry, glm::vec3(0.5));
		TextRenderer::Instance().draw("interact with " + bump->symbol, glm::vec3(1, 1, 1), textTry);
	}
#ifndef  UITEST

	static UiClass ui("./resources/UI/xtm.png");
	static UiClass ui2("./resources/UI/xtk.png");
	//static UiClass ui3("./resources/back.png");
	ui.setPaintArea(-1.0, hp-1.0f, -1.0f, 1.0f);
	ui.setGeometry(32, 32);
	ui2.setGeometry(32, 32);
	tDshader->run();
	//glBindVertexArray(quadArr);
	//ui3.Draw(*tDshader);
	ui2.Draw(*tDshader);
	ui.Draw(*tDshader);
	mouse->Draw(*tDshader);
	
	glDisable(GL_DEPTH_TEST);
	
	//printf("%lf\n", glfwGetTime() - time);
#endif
}

bool key[1024];

void movement()
{
	const GLfloat MaxcameraSpeed = 0.25f;
	static GLfloat cameraSpeed = 0.25f;
	static glm::vec3 lastDir;
	static int delay = 50;
	// temp variable
	if (!hero) return;
	
	glm::vec3 cameraPos = hero->position;
	running = false;
	bump = 0;
	static glm::vec3 cameraWorldPos;
	//host->position = glm::vec3{0,1.5,3} + hero->position;

	if(delay > 0) delay--;

	if (key[GLFW_KEY_W])
	{
		cameraPos += cameraSpeed * hero->forward;
		hero->rotationAngles.z = glm::radians(180.0f);
		lastDir = hero->forward;
		running = true;
	}
	if (key[GLFW_KEY_S]) {
		cameraPos -= cameraSpeed * hero->forward;
		hero->rotationAngles.z = glm::radians(0.0f);
		running = true;
		lastDir = -hero->forward;
	}
	if (key[GLFW_KEY_A])
	{
		cameraPos -= glm::normalize(glm::cross(hero->forward, host->Up)) * cameraSpeed;
		hero->rotationAngles.z = glm::radians(270.0f);
		if(key[GLFW_KEY_W]) hero->rotationAngles.z = glm::radians(225.0f);
		else if (key[GLFW_KEY_S]) hero->rotationAngles.z = glm::radians(-45.0f);
		running = true;
		lastDir = -glm::normalize(glm::cross(hero->forward, host->Up));
	}
	if (key[GLFW_KEY_D])
	{
		cameraPos += glm::normalize(glm::cross(hero->forward, host->Up)) * cameraSpeed;
		hero->rotationAngles.z = glm::radians(90.0f);
		if (key[GLFW_KEY_W]) hero->rotationAngles.z = glm::radians(135.0f);
		else if (key[GLFW_KEY_S]) hero->rotationAngles.z = glm::radians(45.0f);
		running = true;
		lastDir = glm::normalize(glm::cross(hero->forward, host->Up));
	}
	if (key[GLFW_KEY_Q])
	{
		DirLight::max += 0.1f;
		//hero->model->headangle += 0.2f;
	}
	if (key[GLFW_KEY_E])
	{
		DirLight::max -= 0.1f;
		//hero->model->headangle -= 0.2f;
	}
	/*if (!running)
	{
		if (cameraSpeed > 0) {
			cameraSpeed -= 0.05f;
			cameraPos += cameraSpeed * lastDir;
		}
	}*/

	if (key[GLFW_KEY_SPACE])
	{
		if (!delay)
		{
			addExplosion(hero->worldPos() + glm::vec3{ 0,2.0f,0 });
			delay = 50;
		}
	}
		//if (upForce == 0) upForce = 0.3f;
	if (key[GLFW_KEY_Z]) {
		key[GLFW_KEY_Z] = false;
		if (hidden) upForce = 0.28f;
		else if (!hidden) upForce = -0.02f;
		hidden = !hidden;
	}
	/*if (key[GLFW_KEY_Q]) {
		for (int i = 0; i < objects.size(); i++)
		{
			for(int n = 0;n < 3;n++)
				std::cout << objects[i]->position[n] << " ";
			std::cout << "\n";
		}
	}*/
	

	
	if (key[GLFW_KEY_UP])
	{
		changePos.y += 1.0;
	}
	if (key[GLFW_KEY_DOWN])
	{
		changePos.y -= 1.0;
	}
	if (key[GLFW_KEY_LEFT])
	{
		changePos.x -= 1.0;
	}
	if (key[GLFW_KEY_RIGHT])
	{
		changePos.x += 1.0;
	}
	/*if (key[GLFW_KEY_Q]) {
		grasshost->rotationAngles.x += 0.01;
	}
	if (key[GLFW_KEY_E]) {
		grasshost->rotationAngles.y += 0.01;
	}
	if (key[GLFW_KEY_R]) {
		grasshost->rotationAngles.z += 0.01;
	}*/
	//if (key[GLFW_KEY_Q]) std::cout << cameraFront.z << std::endl;
	//if (key[GLFW_KEY_0]) lightPos += cameraSpeed * cameraFront;
	bool collitest = false;
	
	glm::vec3 ori = hero->position;

	float old_y = terrain->getHeight(hero->position.x, hero->position.z); // 位置检测
	float new_y = terrain->getHeight(cameraPos.x, cameraPos.z);

	hero->position = cameraPos;

	for (int i = 1; i < objects.size(); i++)
	{
		collitest |= objects[i]->collisionTest(*hero);
		//if (collitest) return;
	}
	if ((new_y - old_y > 1.0f) || (new_y - old_y < -1.0f))
	{
		int a = 2;
		int b = 3;
		printf("%f, %f\n", new_y, old_y);
	}
	// || (new_y - old_y > 0.5f) || (new_y - old_y < -0.5f)
	if (collitest ) hero->position = ori;
	
}

void key_callback(GLFWwindow* window, int _key, int scancode, int action, int mode)
{
	if (_key >= 0 && _key < 1024)
	{
		if (action == GLFW_PRESS) key[_key] = true;
		else if (action == GLFW_RELEASE) key[_key] = false;
	}
}


bool first = false;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouse->setGeometry(xpos*2, ypos*2);
	mousePos.x
		= xpos;
	mousePos.y 
		= ypos;
	//gluUnProject(xpos, ypos, 0, );
	//return;
	if (!first)
	{
		first = true;
		//lastX = xpos;
		//lastY = ypos;
		return;
	}
	
	GLfloat yoffset = ypos * 2 - Scheight;
	yaw = -glm::sin(yoffset * glm::pi<float>() / 2.0f / Scheight);
	GLfloat xoffset = xpos * 2 - Scwidth;
	pitch = xoffset * glm::pi<float>() / 2.0f / Scwidth;
	/*GLfloat xoffset = lastX - xpos;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top

	GLfloat sensitivity = 0.3f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw = 90.0f + xoffset;
	pitch = yoffset;
	if (pitch > 59.0f)
		pitch = 59.0f;
	if (pitch < -59.0f)
		pitch = -59.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	host->Front = glm::normalize(front);
	front.y = 0;
	hero->forward = glm::normalize(front);*/
	//pitch = pitch + backForce;
	//std::cout << front.x << front.y << front.z << std::endl;
}

void enter_callback(GLFWwindow* window, int _)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	//first = false;
	//lastX = x;
	//lastY = y;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	changeScl -= yoffset > 0 ? 0.001f : -0.001f;
	if (changeScl > 0.05f) changeScl = 0.05f;
	else if (changeScl < 0) changeScl = 0;
	/*if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset / 30;
	if (fov <= 44.5f)
		fov = 44.5f;
	if (fov >= 45.0f)
		fov = 45.0f;*/
	//std::cout << yoffset <<std::endl;
}

void click_callback(GLFWwindow* window, int button, int action, int mods)
{
	/*if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		for (int i = 0;i < 6;i++)
		{
			for (int j = 0; j < 6;j++)
			{
				int Posy = hero->position.x + 24 - 3 + i;
				if (Posy < 0 || Posy > 48) continue;
				int Posx = hero->position.z + 24 - 3 + j;
				if (Posx < 0 || Posx > 48) continue;
				printf("ascend %d %d\n", Posx, Posy);
				waterfloor[Posy * 49 + Posx].Position.y += weights[j][i] * changeScl;
			}
		}
		waterFace->reMesh(waterfloor, 49 * 49);
	}*/
	//if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) backForce = 15.0f;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	

	GLFWwindow* window = glfwCreateWindow(1280, 900, "HelloWorld", nullptr, nullptr);
	const GLubyte *str = glGetString(GL_EXTENSIONS);
	if (str)
		std::cout << str;
	if (window == nullptr)
	{
		std::cout << "Failed to create Window";
		glfwTerminate();
		return -1;
	}
	int yn;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &yn);
	std::cout << "可以使用t？" << yn << std::endl;
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorEnterCallback(window, enter_callback);
	glfwSetMouseButtonCallback(window, click_callback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to Initialize";
		return -1;
	}

	glfwGetFramebufferSize(window, &Scwidth, &Scheight);
	Transform_2D = glm::ortho(
		0.0f,
		(float)Scwidth, 0.0f,
		(float)Scheight, 1.0f, -1.0f);
	glViewport(0, 0, Scwidth, Scheight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glEnable(GL_DOUBLEBUFFER);
	//glEnable(GL_EXT_multisample);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//tasks[0] = new std::thread(animate_load);
	//tasks[1] = new std::thread(animate_load);
	//tasks[2] = new std::thread(animate_load);
	double old_time = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		double now_time = glfwGetTime();
		double dis = now_time - old_time;
		if (dis >= (1.0/40.0))
		{
			old_time = now_time;
			render();
			glfwSwapBuffers(window);
			glfwPollEvents();
			movement();
			
			//double frame = 1.0 / dis;
			//char tmp[128];
			//sprintf(tmp,"Test fps:%4.0f", frame);
			//glfwSetWindowTitle(window, tmp);
		}
		
		//glfwSwapBuffers(window);
	
	}

	glfwTerminate();
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	delete[] waterfloor;
	delete[] waterindex;
	delete waterFace;
	delete ParticleFactory::_shader;
	//delete tasks[0];
	//delete tasks[1];
	//delete tasks[2];
	return 0;
}