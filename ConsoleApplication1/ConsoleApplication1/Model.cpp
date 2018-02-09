#include "Model.h"
#include "CacheManager.h"
#include "include/glm/gtc/type_ptr.hpp"

#include <iostream>

Model::Model(const GLchar* path) : _path(path),anims(0)
{
	Animations.reset(new std::map<std::string,Anims>);
	bone_table.reset(new std::map<std::string, int>);
	mesh_table.reset(new std::map<std::string, int>);
	root.reset(new Node);
	aniBuffer.reset(new _aniBuffer());
	this->load(path);
	std::cout << "Loaded " << path << std::endl;
	CacheManager<Model>::Instance().setResource(path, this);
	
}

Model::Model(const Model & model) : _path(model._path),
	meshes(model.meshes),
	bones(model.bones),
	mesh_table(model.mesh_table),
	bone_table(model.bone_table),
	Animations(model.Animations),
	root(model.root),
	INVERSE(model.INVERSE),
	tickPsec(model.tickPsec),
	finals(model.finals),
	aniBuffer(model.aniBuffer),
	anims(0)
{
	for (int i = 0; i < model.bones.size(); i++)
	{
		bones[i].mat = model.bones[i].mat;
		bones[i].fin = model.bones[i].mat;
	}
	float zero = 0;
	animate("default", zero);
}

void Model::draw(shader& Shader)
{
	char tmp[32];
	Shader.bind("Tinverse", (const GLfloat*)&INVERSE);
	for (int i = 0; i < bones.size(); i++)
	{
		Shader.bind("transforms", (const GLfloat*)&finals[i],i);
	}
	for (GLuint i = 0;i < this->meshes.size();i++)
		this->meshes[i]->Draw(Shader);
}


Model::~Model()
{
	Animations.reset();
	mesh_table.reset();
	root.reset();
	bone_table.reset();
	CacheManager<Model>::Instance().releaseResource(_path);
}

void Model::load(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	cSize = 0;
	INVERSE = scene->mRootNode->mTransformation;
	INVERSE.Inverse();

	this->directory = path.substr(0, path.find_last_of('/'));
	this->processNode(scene->mRootNode, scene);
	generateNode(*root, scene->mRootNode);

	if (scene->mNumAnimations > 0)
	{
		tickPsec = scene->mAnimations[0]->mTicksPerSecond;
		if (tickPsec <= 1) tickPsec = 25;
		//tickPsec *= 25;
		Anims seq;
		_aniBuffer::iterator it = aniBuffer.get()->insert(aniBuffer.get()->end(), std::make_pair("default",
			Anim_Res{ std::vector<std::vector<aiMatrix4x4>>
			(scene->mAnimations[0]->mChannels[0]->mNumPositionKeys),
			std::vector<float> (scene->mAnimations[0]->mChannels[0]->mNumPositionKeys)}));
		for (GLuint i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
		{
			Animation temp;
			auto& channel = scene->mAnimations[0]->mChannels[i];
			auto it2 = bone_table.get()->find(channel->mNodeName.C_Str());
			temp.mPositionKeys.reserve(channel->mNumPositionKeys);
			temp.mRotationKeys.reserve(channel->mNumRotationKeys);
			temp.mScalingKeys.reserve(channel->mNumScalingKeys);
			for (GLuint n = 0; n < channel->mNumPositionKeys; n++)
				temp.mPositionKeys.push_back(channel->mPositionKeys[n]);
			temp.mRotationKeys.reserve(channel->mNumRotationKeys);
			for (GLuint n = 0; n < channel->mNumRotationKeys; n++)
				temp.mRotationKeys.push_back(channel->mRotationKeys[n]);
			temp.mScalingKeys.reserve(channel->mNumScalingKeys);
			for (GLuint n = 0; n < channel->mNumScalingKeys; n++)
				temp.mScalingKeys.push_back(channel->mScalingKeys[n]);
			seq.Seq.insert(seq.Seq.end(),
				std::make_pair(channel->mNodeName.C_Str(),
					temp));

		}
		for (int i = 0;i < it->second.Seq.size();i++)
		{
			it->second.marks[i] = -1;
			it->second.Seq[i].resize(bones.size());
		}
		seq.duration = scene->mAnimations[0]->mDuration;
		Animations.get()->insert(Animations.get()->end(),std::make_pair("default",seq));
		
		
	}
	
	float zero = 0;
	animate("default",zero);
	finals.resize(bones.size());
}

void Model::generateNode(Node& node, aiNode* root)
{
	node.name = root->mName.C_Str();
	node.old = root->mTransformation;
	//printf("%s\n", node.name.data());
	for (int i = 0; i < root->mNumChildren; i++)
	{
		node.children.push_back(Node());
		generateNode(node.children.back(), root->mChildren[i]);
	}
}

const aiMatrix4x4 Identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

/*
void Model::animate(std::string symbol,float& tick)
{
	aiMatrix4x4 tmp;
	// search for the given symbol
	// if an animation is fetched,play it
	std::map<std::string, Anims>::iterator it = Animations.get()->find(symbol);
	if (it == Animations.get()->end())
		return;
	anims = it->second;
	while (tick >= anims.duration*tickPsec) tick -= anims.duration*tickPsec;
	this->animateNode(*root.get(), tick/tickPsec, tmp);
}
*/

const aiMatrix4x4 operator*(const aiMatrix4x4& a, float b)
{
	aiMatrix4x4 res;
	for (int i = 0;i < 4;i++)
		for (int n = 0;n < 4;n++)
			res[i][n] = a[i][n] * b;
	return res;
}

const aiMatrix4x4 operator+(const aiMatrix4x4& a, const aiMatrix4x4& b) {
	aiMatrix4x4 res;
	for (int i = 0;i < 4;i++)
		for (int n = 0;n < 4;n++)
			res[i][n] = a[i][n] + b[i][n];
	return res;
}

void Model::animate(std::string symbol, float& tick)
{
	aiMatrix4x4 tmp;
	// search for the given symbol
	// if an animation is fetched,play it
	std::map<std::string, Anims>::iterator it = Animations.get()->find(symbol);
	if (it == Animations.get()->end())
		return;
	if (anims != &it->second)
	{
		anims = &it->second;
		lasting = anims->duration*tickPsec;
		now_play = symbol;
	}
	
	while (tick >= lasting) tick -= lasting;
	const int index = findKeyRol(tick / tickPsec, anims->Seq.begin()->second);
	float start;
	float end;
	start = end = anims->Seq.begin()->second.mPositionKeys[index].mTime;
	const std::vector<aiMatrix4x4> * matrix1, * matrix2;
	matrix1 = matrix2 = &(aniBuffer->at(symbol).Seq[index]);
	if (aniBuffer->at(symbol).marks[index] < 0);
	{
		aniBuffer->at(symbol).marks[index] = anims->Seq.begin()->second.mPositionKeys[index].mTime;
		this->animateNodeAtIndex(*root.get(), index, tmp, aniBuffer->at(symbol));
	}
	if (index > 0 && aniBuffer->at(symbol).marks[index-1] < 0) {
		aniBuffer->at(symbol).marks[index-1] = anims->Seq.begin()->second.mPositionKeys[index-1].mTime;
		this->animateNodeAtIndex(*root.get(), index-1, tmp, aniBuffer->at(symbol));
		start = anims->Seq.begin()->second.mPositionKeys[index - 1].mTime;
		matrix1 = &(aniBuffer->at(symbol).Seq[index-1]);
	}
	else if (index > 0)
	{
		start = anims->Seq.begin()->second.mPositionKeys[index - 1].mTime;
		matrix1 = &(aniBuffer->at(symbol).Seq[index - 1]);
	}
	float dura = end - start;
	float frac = (tick / tickPsec - start) / (end - start);
	for (int i = 0;i < bones.size();i++)
	{
		bones[i].fin = matrix1->at(i) * (1 - frac) + matrix2->at(i) * frac;
	}
}

void Model::animate(float & tick)
{
	aiMatrix4x4 tmp;
	// search for the given symbol
	// if an animation is fetched,play it
	std::string symbol = now_play;
	while (tick >= lasting) tick -= lasting;
	const int index = findKeyRol(tick / tickPsec, anims->Seq.begin()->second);
	float start;
	float end;
	start = end = anims->Seq.begin()->second.mPositionKeys[index].mTime;
	const std::vector<aiMatrix4x4> * matrix1, *matrix2;
	matrix1 = matrix2 = &(aniBuffer->at(symbol).Seq[index]);
	if (aniBuffer->at(symbol).marks[index] < 0);
	{
		aniBuffer->at(symbol).marks[index] = anims->Seq.begin()->second.mPositionKeys[index].mTime;
		this->animateNodeAtIndex(*root.get(), index, tmp, aniBuffer->at(symbol));
	}
	if (index > 0 && aniBuffer->at(symbol).marks[index - 1] < 0) {
		aniBuffer->at(symbol).marks[index - 1] = anims->Seq.begin()->second.mPositionKeys[index - 1].mTime;
		this->animateNodeAtIndex(*root.get(), index - 1, tmp, aniBuffer->at(symbol));
		start = anims->Seq.begin()->second.mPositionKeys[index - 1].mTime;
		matrix1 = &(aniBuffer->at(symbol).Seq[index - 1]);
	}
	else if (index > 0)
	{
		start = anims->Seq.begin()->second.mPositionKeys[index - 1].mTime;
		matrix1 = &(aniBuffer->at(symbol).Seq[index - 1]);
	}
	float dura = end - start;
	float frac = (tick / tickPsec - start) / (end - start);
	for (int i = 0;i < bones.size();i++)
	{
		bones[i].fin = matrix1->at(i) * (1 - frac) + matrix2->at(i) * frac;
	}
}

void Model::blend(std::string end, float & tick, float percent)
{
	aiMatrix4x4 tmp;
	// search for the given symbol
	// if an animation is fetched,play it
	std::map<std::string, Anims>::iterator it = Animations.get()->find(end);
	if (it == Animations.get()->end())
		return;
	blending = it->second;
	animate(tick);
	if (aniBuffer->at(end).marks[0] < 0)
	{
		aniBuffer->at(end).marks[0] = blending.Seq.begin()->second.mPositionKeys[0].mTime;
		blendNodeAtIndex(*root.get(), 0, tmp, aniBuffer->at(end));
	}
	std::vector<aiMatrix4x4>* matrix2 = &aniBuffer->at(end).Seq[0];
	for (int i = 0;i < bones.size();i++)
	{
		bones[i].fin = bones[i].fin * (1 - percent) + matrix2->at(i) * percent;
	}
	//while (tick >= anims->duration*tickPsec) tick -= anims->duration*tickPsec;
	//this->blendNode(*root.get(), tick / tickPsec,percent, tmp);
}

void Model::registAnimation(std::string filename, std::string symbol)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	if (scene->mNumAnimations > 0)
	{
		Anims seq;
		_aniBuffer::iterator it = aniBuffer.get()->insert(aniBuffer.get()->end(), std::make_pair(symbol,
			Anim_Res{ std::vector<std::vector<aiMatrix4x4>>
			(scene->mAnimations[0]->mChannels[0]->mNumPositionKeys),
			std::vector<float>(scene->mAnimations[0]->mChannels[0]->mNumPositionKeys) }));
		for (GLuint i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
		{
			Animation temp;
			auto& channel = scene->mAnimations[0]->mChannels[i];
			auto it2 = bone_table.get()->find(channel->mNodeName.C_Str());
			temp.mPositionKeys.reserve(channel->mNumPositionKeys);
			temp.mRotationKeys.reserve(channel->mNumRotationKeys);
			temp.mScalingKeys.reserve(channel->mNumScalingKeys);
			for (GLuint n = 0; n < channel->mNumPositionKeys; n++)
				temp.mPositionKeys.push_back(channel->mPositionKeys[n]);
			temp.mRotationKeys.reserve(channel->mNumRotationKeys);
			for (GLuint n = 0; n < channel->mNumRotationKeys; n++)
				temp.mRotationKeys.push_back(channel->mRotationKeys[n]);
			temp.mScalingKeys.reserve(channel->mNumScalingKeys);
			for (GLuint n = 0; n < channel->mNumScalingKeys; n++)
				temp.mScalingKeys.push_back(channel->mScalingKeys[n]);
			seq.Seq.insert(seq.Seq.end(),
				std::make_pair(channel->mNodeName.C_Str(),
					temp));

		}
		for (int i = 0;i < it->second.Seq.size();i++)
		{
			it->second.marks[i] = -1;
			it->second.Seq[i].resize(bones.size());
		}
		seq.duration = scene->mAnimations[0]->mDuration;
		Animations.get()->insert(Animations.get()->end(), std::make_pair(symbol, seq));
		
	}
}

#ifndef HH
void Model::animateNode(const Node & RootNode, float tick,const aiMatrix4x4 & pMat)
{
	std::map<std::string, Animation>::iterator it = anims->Seq.find(RootNode.name.data());
	aiQuaternion Out;
	aiMatrix4x4 nextMat,tranMat;
	aiVector3D posO, sclO;
	nextMat = pMat;
	tranMat =  RootNode.old;
	if (it != anims->Seq.end()) {
		const Animation& anim = it->second;
		/* Rot */
		GLuint key = findKeyRol(tick, anim);
		GLuint key2 = key - 1;
		if (key == anim.mRotationKeys.size()) key--;
		if (key2 < 0) key2 = 0;
		float dura = anim.mRotationKeys[key].mTime - anim.mRotationKeys[key2].mTime;
		float frac = (tick - anim.mRotationKeys[key2].mTime) / dura;
		const aiQuaternion& StartRotationQ = anim.mRotationKeys[key2].mValue;
		const aiQuaternion& EndRotationQ = anim.mRotationKeys[key].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, frac);
		Out.Normalize();

		/* Pos */
		//key = findKeyPos(tick, anim);
		//key2 = key - 1;
		if (key == anim.mPositionKeys.size()) key--;
		if (key2 < 0) key2 = 0;
		dura = anim.mPositionKeys[key].mTime - anim.mPositionKeys[key2].mTime;
		frac = (tick - anim.mPositionKeys[key2].mTime) / dura;
		const aiVector3D& StartPQ = anim.mPositionKeys[key2].mValue;
		const aiVector3D& EndPQ = anim.mPositionKeys[key].mValue;
		posO = StartPQ * frac + EndPQ * (1-frac);

		/* Scale */
		//key = findKeySca(tick, anim);
		//key2 = key - 1;
		if (key == anim.mScalingKeys.size()) key--;
		if (key2 < 0) key2 = 0;
		dura = anim.mScalingKeys[key].mTime - anim.mScalingKeys[key2].mTime;
		frac = (tick - anim.mScalingKeys[key2].mTime) / dura;
		const aiVector3D& StartSQ = anim.mScalingKeys[key2].mValue;
		const aiVector3D& EndSQ = anim.mScalingKeys[key].mValue;
		sclO = StartSQ * frac + EndSQ * (1 - frac);

		aiMatrix4x4 _nextMat(sclO, Out, posO);
		tranMat = _nextMat;
		
	}
	aiMatrix4x4 globalTransform = pMat * tranMat;

	std::map<std::string, int>::iterator boneId = bone_table.get()->find(RootNode.name.data());

	if (boneId != bone_table.get()->end())
	{
		bones.at(boneId->second).fin = globalTransform * bones.at(boneId->second).mat;
	}

	for(int i = 0;i < RootNode.children.size();i++)
		animateNode(RootNode.children[i], tick, globalTransform);
}

void Model::blendNode(const Node & RootNode, float tick, float percent, const aiMatrix4x4 & pMat)
{
	std::map<std::string, Animation>::iterator it = anims->Seq.find(RootNode.name.data());
	std::map<std::string, Animation>::iterator it2 = blending.Seq.find(RootNode.name.data());
	aiQuaternion Out;
	aiMatrix4x4 nextMat, tranMat;
	aiVector3D posO, sclO;
	nextMat = pMat;
	tranMat = RootNode.old;
	if (it != anims->Seq.end()) {
		const Animation& anim = it->second;
		const Animation& anim2 = it2->second;
		/* Rot */
		GLuint key = findKeyRol(tick, anim);
		GLuint key2 = key - 1;
		if (key == anim.mRotationKeys.size()) key--;
		if (key2 < 0) key2 = 0;
		float dura = anim.mRotationKeys[key].mTime - anim.mRotationKeys[key2].mTime;
		float frac = (tick - anim.mRotationKeys[key2].mTime) / dura;
		const aiQuaternion& StartRotationQ = anim.mRotationKeys[key2].mValue;
		const aiQuaternion& EndRotationQ = anim.mRotationKeys[key].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, frac);

		/* Pos */
		//key = findKeyPos(tick, anim);
		//key2 = key - 1;
		if (key == anim.mPositionKeys.size()) key--;
		if (key2 < 0) key2 = 0;
		//dura = anim.mPositionKeys[key].mTime - anim.mPositionKeys[key2].mTime;
		//frac = (tick - anim.mPositionKeys[key2].mTime) / dura;
		const aiVector3D& StartPQ = anim.mPositionKeys[key2].mValue;
		const aiVector3D& EndPQ = anim.mPositionKeys[key].mValue;
		posO = StartPQ * frac + EndPQ * (1 - frac);

		/* Scale */
		//key = findKeySca(tick, anim);
		//key2 = key - 1;
		if (key == anim.mScalingKeys.size()) key--;
		if (key2 < 0) key2 = 0;
		//dura = anim.mScalingKeys[key].mTime - anim.mScalingKeys[key2].mTime;
		//frac = (tick - anim.mScalingKeys[key2].mTime) / dura;
		const aiVector3D& StartSQ = anim.mScalingKeys[key2].mValue;
		const aiVector3D& EndSQ = anim.mScalingKeys[key].mValue;
		sclO = StartSQ * frac + EndSQ * (1 - frac);

		/*find blending key*/
		const aiQuaternion& EndRotationQ2 = anim2.mRotationKeys[0].mValue;
		aiQuaternion::Interpolate(Out, Out, EndRotationQ2, percent);
		Out.Normalize();

		const aiVector3D& EndPQ2 = anim2.mPositionKeys[0].mValue;
		posO *= (1 - percent);
		posO += EndPQ2 * percent;

		const aiVector3D& EndSQ2 = anim2.mScalingKeys[0].mValue;
		sclO *= (1 - percent);
		sclO += EndSQ2 * percent;

		aiMatrix4x4 _nextMat(sclO, Out, posO);
		tranMat = _nextMat;

	}
	aiMatrix4x4 globalTransform = pMat * tranMat;

	std::map<std::string, int>::iterator boneId = bone_table.get()->find(RootNode.name.data());

	if (boneId != bone_table.get()->end())
	{
		bones.at(boneId->second).fin = globalTransform * bones.at(boneId->second).mat;
	}

	for (int i = 0; i < RootNode.children.size(); i++)
		blendNode(RootNode.children[i], tick, percent, globalTransform);
}

void Model::blendNodeAtIndex(const Node & RootNode, const int index, const aiMatrix4x4 & pMat, Anim_Res & res)
{
	std::map<std::string, Animation>::iterator it = blending.Seq.find(RootNode.name.data());
	aiQuaternion Out;
	aiMatrix4x4 nextMat, tranMat;
	nextMat = pMat;
	tranMat = RootNode.old;
	if (it != blending.Seq.end()) {
		const Animation& anim = it->second;
		const aiQuaternion& RotationQ = anim.mRotationKeys[index].mValue;
		Out = RotationQ;
		Out.Normalize();

		const aiVector3D& PQ = anim.mPositionKeys[index].mValue;

		/* Scale */
		//key = findKeySca(tick, anim);
		//key2 = key - 1;

		const aiVector3D& SQ = anim.mScalingKeys[index].mValue;

		aiMatrix4x4 _nextMat(SQ, Out, PQ);
		tranMat = _nextMat;

	}
	aiMatrix4x4 globalTransform = pMat * tranMat;

	std::map<std::string, int>::iterator boneId = bone_table.get()->find(RootNode.name.data());

	if (boneId != bone_table.get()->end())
	{
		res.Seq[index][boneId->second] = globalTransform * bones.at(boneId->second).mat;
		//bones.at(boneId->second).fin = globalTransform * bones.at(boneId->second).mat;
	}

	for (int i = 0;i < RootNode.children.size();i++)
		blendNodeAtIndex(RootNode.children[i], index, globalTransform, res);
}

GLuint Model::findKeyRol(float tick, const Animation & anim)
{
	
	for (GLuint i = 0; i < anim.mRotationKeys.size(); i++)
	{
		if (anim.mRotationKeys[i].mTime > tick) return i;
	}
	return anim.mRotationKeys.size();
}

GLuint Model::findKeyPos(float tick, const Animation & anim)
{
	for (GLuint i = 0; i < anim.mPositionKeys.size(); i++)
	{
		if (anim.mPositionKeys[i].mTime > tick) return i;
	}
	return anim.mPositionKeys.size();
}

GLuint Model::findKeySca(float tick, const Animation & anim)
{
	for (GLuint i = 0; i < anim.mScalingKeys.size(); i++)
	{
		if (anim.mScalingKeys[i].mTime > tick) return i;
	}
	return anim.mScalingKeys.size();
}
#endif

void Model::processNode(aiNode * RootNode,const aiScene * scene)
{
	for (GLuint i = 0;i < RootNode->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[RootNode->mMeshes[i]];
		this->meshes.push_back(std::shared_ptr<Mesh>(this->generateMesh(mesh,scene,cSize)));
		this->mesh_table.get()->insert(mesh_table.get()->end(), std::make_pair(RootNode->mName.C_Str(),
			i));
	}

	for (GLuint i = 0;i < RootNode->mNumChildren;i++)
		this->processNode(RootNode->mChildren[i], scene);
}

void Model::animateNodeAtIndex(const Node & RootNode, const int index, const aiMatrix4x4 & pMat, Anim_Res & res)
{
	std::map<std::string, Animation>::iterator it = anims->Seq.find(RootNode.name.data());
	aiQuaternion Out;
	aiMatrix4x4 nextMat, tranMat;
	nextMat = pMat;
	tranMat = RootNode.old;
	if (it != anims->Seq.end()) {
		const Animation& anim = it->second;
		const aiQuaternion& RotationQ = anim.mRotationKeys[index].mValue;
		Out = RotationQ;
		Out.Normalize();

		const aiVector3D& PQ = anim.mPositionKeys[index].mValue;

		/* Scale */
		//key = findKeySca(tick, anim);
		//key2 = key - 1;
		
		const aiVector3D& SQ = anim.mScalingKeys[index].mValue;

		aiMatrix4x4 _nextMat(SQ, Out, PQ);
		tranMat = _nextMat;

	}
	aiMatrix4x4 globalTransform = pMat * tranMat;

	std::map<std::string, int>::iterator boneId = bone_table.get()->find(RootNode.name.data());

	if (boneId != bone_table.get()->end())
	{
		res.Seq[index][boneId->second] = globalTransform * bones.at(boneId->second).mat;
		//bones.at(boneId->second).fin = globalTransform * bones.at(boneId->second).mat;
	}

	for (int i = 0;i < RootNode.children.size();i++)
		animateNodeAtIndex(RootNode.children[i], index, globalTransform, res );
}

void Model::prepare()
{
	for (int i = 0;i < bones.size();i++)
		finals[i] = bones[i].fin;
}

Mesh* Model::generateMesh(aiMesh * content, const aiScene * scene, const int BaseIndex)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	std::string tmp;
	int bs = content->mNumBones;
	std::map<std::string, _Bone>::iterator it;
	vertices.resize(content->mNumVertices);
	int *bSize = new int[content->mNumVertices];
	memset(bSize, 0, sizeof(int)*content->mNumVertices);
	for (GLuint i = 0; i < content->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.Position = {
			content->mVertices[i].x,
			content->mVertices[i].y,
			content->mVertices[i].z
		};
		
		vertex.Normal = {
			content->mNormals[i].x,
			content->mNormals[i].y,
			content->mNormals[i].z
		};
		if (content->mTextureCoords[0])
		{
			vertex.TexCoords = {
				content->mTextureCoords[0][i].x,
				content->mTextureCoords[0][i].y
			};
		}
		else
			vertex.TexCoords = glm::vec2(0);
		vertex.BIDS = { 0,0,0,0 };
		vertex.WEIGHTS = { 0,0,0,0 };
		vertices[i] = (vertex);
	}
	cSize += content->mNumVertices;
	//bones.reserve(bs);
	for (GLuint i = 0; i < bs; i++)
	{
		tmp = content->mBones[i]->mName.C_Str();
		GLuint boneIndex;
		if (bone_table.get()->find(tmp) == bone_table.get()->end()) {
			bones.push_back(_Bone{
				content->mBones[i]->mOffsetMatrix,
				content->mBones[i]->mOffsetMatrix });
			boneIndex = bones.size() - 1;
			bone_table.get()->insert(bone_table.get()->end(), std::make_pair(tmp, boneIndex));
		}
		else boneIndex = bone_table.get()->at(tmp);
		for (GLuint n = 0; n < content->mBones[i]->mNumWeights; n++)
		{
			auto as = content->mBones[i]->mWeights[n];
			GLuint fiIndex = as.mVertexId + BaseIndex;
			vertices[as.mVertexId].BIDS[bSize[as.mVertexId]] = boneIndex;
			vertices[as.mVertexId].WEIGHTS[bSize[as.mVertexId]] = as.mWeight;
			bSize[as.mVertexId]++;
		}
		std::cout << "-Bone : " << tmp << content->mBones[i]->mWeights->mVertexId 
			<< " support " << content->mBones[i]->mNumWeights << std::endl;
	}
	delete[] bSize;
	for (GLuint i = 0; i < content->mNumFaces;i++)
	{
		aiFace face = content->mFaces[i];
		for (GLuint j = 0;j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	ai_real shininess = 16.0f;
	bool normalhas=false,opacityhas = false;
	if (content->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[content->mMaterialIndex];
		std::vector<std::shared_ptr<Texture>> diffuseMaps, specularMaps,normalMaps,opacityMaps;
		diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		specularMaps = loadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_specular");
		normalMaps = loadMaterialTextures(material,
			aiTextureType_HEIGHT, "normalMap");
		opacityMaps = loadMaterialTextures(material,
			aiTextureType_OPACITY, "opacityMap");
		textures.insert(textures.end(), diffuseMaps.begin(),diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
		if (normalMaps.size() > 0)
			normalhas = true;
		if (opacityMaps.size() > 0)
			opacityhas = true;
		material->Get(AI_MATKEY_SHININESS, shininess);
	}

	Mesh* tempMesh = new Mesh(vertices,indices,textures);
	tempMesh->shininess = shininess;
	tempMesh->normalhas = normalhas;
	tempMesh->opacityhas = opacityhas;
	tempMesh->BaseID = BaseIndex;
	return tempMesh;
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<std::shared_ptr<Texture>> textures;
	
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		if (std::string(str.C_Str()) == "*0") str.Set("0.dds");

		std::cout << " Try to Fetch " << str.C_Str() << std::endl;
		std::string path = (this->directory + str.C_Str());
		Texture* find = CacheManager<Texture>::Instance().getResource(path);
		if (find)
		{
			textures.push_back(std::shared_ptr<Texture>(find));
		}
		else
		{
			Texture *texture  =  new Texture(path.data());
			//std::cout << path.data() << std::endl;
			if(type == aiTextureType_NORMALS)
			std::cout << path.data() << " is created at " << texture->id() << std::endl;
			texture->type = typeName;
			texture->path = path;
			textures.push_back(std::shared_ptr<Texture>(texture));
			CacheManager<Texture>::Instance().setResource(path, texture);
		}
	}
	return textures;
}