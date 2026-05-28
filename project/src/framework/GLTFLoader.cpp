#include "GLTFLoader.h"

#include <iostream>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

dae::Scene dae::GLTFLoader::LoadFile(const std::string& path)
{
	//Load GLTF file
	Assimp::Importer importer{};
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices
	);

	Scene outScene{};

	// --- Materials -----------------------------
	for (uint32_t idx{}; idx < scene->mNumMaterials; ++idx)
	{
		ProcessMaterial(scene->mMaterials[idx], scene, outScene);
	}

	// --- Meshes --------------------------------
	for (uint32_t idx{}; idx < scene->mNumMeshes; ++idx)
	{
		ProcessMesh(scene->mMeshes[idx], scene, outScene);
	}

	outScene.rootNode = ProcessNode(scene->mRootNode, scene, outScene);


	std::cout << "Scene has " <<  scene->mNumMeshes << " meshes" << "\n";
	std::cout << "Scene has " <<  scene->mNumTextures << " textures" << "\n";
	std::cout << "Scene has " <<  scene->mNumMaterials << " materials" << "\n";
	return outScene;
}

uint32_t dae::GLTFLoader::ProcessNode(aiNode* node, const aiScene* scene, Scene& outScene)
{
}

uint32_t dae::GLTFLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, Scene& outScene)
{
}

uint32_t dae::GLTFLoader::ProcessMaterial(aiMaterial* mat, const aiScene* scene, Scene& outScene)
{
}

uint32_t dae::GLTFLoader::LoadTexture(aiString path, Scene& outScene)
{
}

glm::mat4 dae::GLTFLoader::ConvertMatrix(const aiMatrix4x4& mat)
{
}
