#include "SModel.h"
#include "CacheManager.h"
#include "include/glm/gtc/type_ptr.hpp"

#include <iostream>

SModel::SModel(const GLchar* path) : 
	_path(path)
{
	this->load(path);
	std::cout << "Loaded " << path << std::endl;
	CacheManager<SModel>::Instance().setResource(path, this);

}

SModel::SModel(const SModel & model) : 
	_path(model._path),
	meshes(model.meshes)
{
}

void SModel::draw(shader& Shader)
{
	char tmp[32];

	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i]->Draw(Shader);
}

void SModel::draw(shader& Shader,int repeat)
{
	char tmp[32];

	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i]->Draw(Shader,repeat);
}


SModel::~SModel()
{
	CacheManager<SModel>::Instance().releaseResource(_path);
}

void SModel::load(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	this->directory = path.substr(0, path.find_last_of('/'));
	this->processNode(scene->mRootNode, scene);
}

void SModel::processNode(aiNode * RootNode, const aiScene * scene)
{
	for (GLuint i = 0; i < RootNode->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[RootNode->mMeshes[i]];
		this->meshes.push_back(std::shared_ptr<SMesh>(this->generateMesh(mesh, scene, 0)));
	}

	for (GLuint i = 0; i < RootNode->mNumChildren; i++)
		this->processNode(RootNode->mChildren[i], scene);
}

SMesh* SModel::generateMesh(aiMesh * content, const aiScene * scene, const int BaseIndex)
{
	std::vector<SVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	vertices.resize(content->mNumVertices);
	for (GLuint i = 0; i < content->mNumVertices; i++)
	{
		SVertex vertex;
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
		vertices[i] = (vertex);
	}
	for (GLuint i = 0; i < content->mNumFaces; i++)
	{
		aiFace face = content->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	ai_real shininess = 16.0f;
	bool normalhas = false, opacityhas = false;
	if (content->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[content->mMaterialIndex];
		std::vector<std::shared_ptr<Texture>> diffuseMaps, specularMaps, normalMaps, ambientMaps, opacityMaps;
		diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");

		normalMaps = loadMaterialTextures(material,
			aiTextureType_HEIGHT, "normalMap");
		opacityMaps = loadMaterialTextures(material,
			aiTextureType_OPACITY, "opacityMap");
		ambientMaps = loadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_ambient");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
		textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
		if (normalMaps.size() > 0)
			normalhas = true;
		if (opacityMaps.size() > 0)
			opacityhas = true;
		material->Get(AI_MATKEY_SHININESS, shininess);
	}

	SMesh* tempMesh = new SMesh(vertices, indices, textures);
	tempMesh->shininess = shininess;
	tempMesh->normalhas = normalhas;
	tempMesh->opacityhas = opacityhas;
	return tempMesh;
}

std::vector<std::shared_ptr<Texture>> SModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<std::shared_ptr<Texture>> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		if (std::string(str.C_Str()) == "*0") str.Set("0.dds");

		std::cout << " Try to Fetch " << str.C_Str() << std::endl;
		std::string tmp = str.C_Str();
		while (tmp[0] == '.' || tmp[0] == '/') tmp = tmp.data() + 1;
		std::string path = (this->directory + "/" + tmp);
		Texture* find = CacheManager<Texture>::Instance().getResource(path);
		if (find)
		{
			textures.push_back(std::shared_ptr<Texture>(find));
		}
		else
		{
			Texture *texture = new Texture(path.data());
			//std::cout << path.data() << std::endl;
			if (type == aiTextureType_NORMALS)
				std::cout << path.data() << " is created at " << texture->id() << std::endl;
			texture->type = typeName;
			texture->path = path;
			textures.push_back(std::shared_ptr<Texture>(texture));
			CacheManager<Texture>::Instance().setResource(path, texture);
		}
	}
	return textures;
}