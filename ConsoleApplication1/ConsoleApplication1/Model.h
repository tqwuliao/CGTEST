#pragma once

#include "include\assimp\Importer.hpp"
#include "include\assimp\scene.h"
#include "include\assimp\postprocess.h"

#include <map>
#include "Mesh.h"

struct _Bone {
	aiMatrix4x4 mat;
	aiMatrix4x4 fin;
};

struct Node {
	std::string name;
	aiMatrix4x4 old;
	std::vector<Node> children;
};

struct Animation {
	std::vector<aiVectorKey> mPositionKeys, mScalingKeys;
	std::vector<aiQuatKey> mRotationKeys;
};

struct Anims {
	std::map<std::string, Animation> Seq;
	float duration;
};

// -| all nodes have the same keys
// --| keys
// --| key n
// ---| node a
// ---| node b....
struct Anim_Res {
	std::vector<std::vector<aiMatrix4x4>> Seq;
	std::vector<float> marks; // if a mark is -1, means this has not been calculated yet.
};

class Model
{
public:
	std::vector<std::shared_ptr<Mesh>> meshes; // Mesh reuseable.
	float headangle=0;
	Model(const GLchar* path);
	Model(const Model& model);
	typedef std::map < std::string, Anim_Res > _aniBuffer;
	std::shared_ptr<_aniBuffer> aniBuffer;
	void draw(shader& Shader);
	void draw(shader & Shader, int repeat);
	void animate(std::string symbol,float & tick);
	void animate(float & tick);
	void blend(std::string end, float & tick, float percent);
	void registAnimation(std::string filename, std::string symbol);
	void prepare();
	~Model();
private:
	Anims* anims; // The animation being played right now
	Anims blending; // The destination of a blending.
	GLuint cSize;
	int headPos;
	std::string directory;
	std::string _path;
	std::string now_play;
	//std::string now_play;
	/* 
	Animations : store the information that corresponse to a dir
	reuseable,using pointer 
	*/
	std::shared_ptr<std::map<std::string, Anims>> Animations; 
	/*
	Mesh_table : store the mesh information that corresponse to a single skeleton
	reuseable,using pointer
	*/
	std::shared_ptr<std::map<std::string, int>> mesh_table;
	/*
	Bone_table : store the bone information that corresponse to a single skeleton
	reuseable,using pointer
	*/
	std::shared_ptr<std::map<std::string, int>> bone_table;
	/*
	Bones : store the bone information that corresponse to a single skeleton
	not reuseable
	*/
	std::vector<_Bone> bones;
	std::vector<aiMatrix4x4> finals;
	aiMatrix4x4 INVERSE;
	void load(std::string path);
	void generateNode(Node & node, aiNode * root);
	const aiScene* scene;
	float lasting;
	//const aiNode* root;
	std::shared_ptr<Node> root;
	void animateNode(const Node & RootNode, float tick, const aiMatrix4x4 & pMat);
	void blendNode(const Node & RootNode, float tick, float percent, const aiMatrix4x4 & pMat);
	void blendNodeAtIndex(const Node & RootNode, const int index, const aiMatrix4x4 & pMat, Anim_Res & res);
	GLuint findKeyRol(float tick, const Animation & anim);
	GLuint findKeyPos(float tick, const Animation & anim);
	GLuint findKeySca(float tick, const Animation & anim);
	void processNode(aiNode* RootNode,const aiScene* scene);
	void animateNodeAtIndex(const Node & RootNode, const int index, const aiMatrix4x4 & pMat, Anim_Res & res);
	Mesh* generateMesh(aiMesh* content,const aiScene* scene, const int BaseIndex);
	float tickPsec;
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
};

