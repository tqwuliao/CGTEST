#pragma once

#include "include\assimp\Importer.hpp"
#include "include\assimp\scene.h"
#include "include\assimp\postprocess.h"

#include <map>
#include "SMesh.h"

class SModel
{
public:
	std::vector<std::shared_ptr<SMesh>> meshes; // Mesh reuseable.
	SModel(const GLchar* path);
	SModel(const SModel& model);
	void draw(shader& Shader);
	void draw(shader & Shader, int repeat);
	~SModel();
private:
	std::string directory;
	std::string _path;
	void load(std::string path);
	void processNode(aiNode * RootNode, const aiScene * scene);
	SMesh* generateMesh(aiMesh* content, const aiScene* scene, const int BaseIndex);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
};

