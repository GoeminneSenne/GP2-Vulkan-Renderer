#include "GLTFLoader.h"

#include <iostream>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include "ImageHelpers.h"
#include "Vertex.h"

dae::Scene dae::GLTFLoader::LoadScene(const Context& context, const std::string& path)
{
	m_sceneDirectory = std::filesystem::path(path).parent_path();

	// --- Load GLTF file ------------------------
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
		ProcessMaterial(context, scene->mMaterials[idx], scene, outScene);
	}

	// --- Meshes --------------------------------
	for (uint32_t idx{}; idx < scene->mNumMeshes; ++idx)
	{
		ProcessMesh(context, scene->mMeshes[idx], scene, outScene);
	}

	outScene.rootNode = ProcessNode(context, scene->mRootNode, scene, outScene);
	auto& rootTransform = outScene.nodes[outScene.rootNode].transform;
	rootTransform = GLTF_CONVERSION_MATRIX * rootTransform;

	std::cout << "Scene has " <<  scene->mNumMeshes << " meshes" << "\n";
	std::cout << "Scene has " <<  scene->mNumMaterials << " materials" << "\n";

	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			std::cout << rootTransform[col][row] << " ";
		}
		std::cout << '\n';
	}
	return outScene;
}

uint32_t dae::GLTFLoader::ProcessNode(const Context& context, aiNode* aiNode, const aiScene* scene, Scene& outScene)
{
	Node node{};

	// --- Transform ------------------------
	node.transform = ConvertMatrix(aiNode->mTransformation);

	// --- Mesh References ------------------
	for (uint32_t idx{}; idx < aiNode->mNumMeshes; ++idx)
	{
		uint32_t meshIndex = aiNode->mMeshes[idx];
		std::cout << meshIndex << "\n";
	
		node.meshIndices.push_back(meshIndex);
	}

	// --- Child Nodes ---------------------
	for (uint32_t idx{}; idx < aiNode->mNumChildren; ++idx)
	{
		uint32_t childIndex = ProcessNode(context, aiNode->mChildren[idx], scene, outScene);
		node.children.push_back(childIndex);
	}

	outScene.nodes.push_back(node);
	return static_cast<uint32_t>(outScene.nodes.size() - 1);
}

uint32_t dae::GLTFLoader::ProcessMesh(const Context& context, aiMesh* aiMesh, const aiScene* scene, Scene& outScene)
{
	Mesh mesh{};
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};

	vertices.reserve(aiMesh->mNumVertices);
	indices.resize(aiMesh->mNumFaces * 3);

	// --- Vertices ------------------------
	for (uint32_t idx{}; idx < aiMesh->mNumVertices; ++idx)
	{
		Vertex vertex{};

		vertex.pos = 
		{
			aiMesh->mVertices[idx].x,
			aiMesh->mVertices[idx].y,
			aiMesh->mVertices[idx].z
		};

		if (aiMesh->HasTextureCoords(0))
		{
			vertex.texCoord =
			{
				aiMesh->mTextureCoords[0][idx].x,
				aiMesh->mTextureCoords[0][idx].y
			};
		}

		vertices.push_back(vertex);
	}

	// --- Indices ------------------------
	for (uint32_t faceIdx{}; faceIdx < aiMesh->mNumFaces; ++faceIdx)
	{
		const aiFace face = aiMesh->mFaces[faceIdx];

		for (uint32_t idx{}; idx < face.mNumIndices; ++idx)
		{
			indices.push_back(face.mIndices[idx]);
		}
	}

	// --- Mesh construction ------------------------

	std::cout << "Creating Mesh" << "\n";

	mesh.materialIndex = aiMesh->mMaterialIndex;
	mesh.Upload(context, vertices, indices);
	outScene.meshes.push_back(mesh);
	return static_cast<uint32_t>(outScene.meshes.size() - 1);
}

uint32_t dae::GLTFLoader::ProcessMaterial(const Context& context, aiMaterial* aiMat, const aiScene* scene, Scene& outScene)
{
	aiString texName;
	if (aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &texName) == AI_SUCCESS)
	{
		auto fullPath = m_sceneDirectory / texName.C_Str();

		//std::cout << "Texture path:  " << fullPath << "\n";

		
		//Image texture;
		//ImageHelpers::CreateTexture(context, fullPath, texture);
	}


	return 0;
}

uint32_t dae::GLTFLoader::LoadTexture(const Context& context, aiString path, Scene& outScene)
{
	return 0;
}

glm::mat4 dae::GLTFLoader::ConvertMatrix(const aiMatrix4x4& mat)
{
	
	return glm::mat4{
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	};
	 

	/*
	return glm::mat4
	{
		mat.a1, mat.a2, mat.a3, mat.a4,
		mat.b1, mat.b2, mat.b3, mat.b4,
		mat.c1, mat.c2, mat.c3, mat.c4,
		mat.d1, mat.d2, mat.d3, mat.d4
	};
	*/
}
