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
	std::cout << "Scene has " << outScene.textures.size() << " textures\n";

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

	//Used to translate vertex normals & tangents to correct axis system
	constexpr glm::mat3 conversion = glm::mat3(GLTF_CONVERSION_MATRIX);

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
				1.f - aiMesh->mTextureCoords[0][idx].y
			};
		}

		if (aiMesh->HasNormals())
		{
			vertex.normal =
			{
				aiMesh->mNormals[idx].x,
				aiMesh->mNormals[idx].y,
				aiMesh->mNormals[idx].z
			};

			vertex.normal = glm::normalize(conversion * vertex.normal);
		}

		if (aiMesh->HasTangentsAndBitangents())
		{
			vertex.tangent =
			{
				aiMesh->mTangents[idx].x,
				aiMesh->mTangents[idx].y,
				aiMesh->mTangents[idx].z
			};

			vertex.tangent = glm::normalize(conversion * vertex.tangent);
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
	mesh.materialIndex = aiMesh->mMaterialIndex;
	mesh.textureIndex = outScene.materials[mesh.materialIndex].baseColor;

	mesh.Upload(context, vertices, indices);
	outScene.meshes.push_back(mesh);
	return static_cast<uint32_t>(outScene.meshes.size() - 1);
}

uint32_t dae::GLTFLoader::ProcessMaterial(const Context& context, aiMaterial* aiMat, const aiScene* scene, Scene& outScene)
{
	Material mat{};

	aiString texName;
	if (aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &texName) == AI_SUCCESS)
	{
		auto fullPath = m_sceneDirectory / texName.C_Str();
		mat.baseColor = LoadTexture(context, fullPath, outScene);
	}

	outScene.materials.push_back(mat);
	return static_cast<uint32_t>(outScene.meshes.size() - 1);
}

uint32_t dae::GLTFLoader::LoadTexture(const Context& context, const std::filesystem::path& path, Scene& outScene)
{
	if (m_textureLookup.contains(path))
	{
		return m_textureLookup[path];
	}

	Image tex;
	ImageHelpers::CreateTexture(context, path, tex);
	outScene.textures.push_back(tex);

	uint32_t texIndex = static_cast<uint32_t>(outScene.textures.size() - 1);
	m_textureLookup[path] = texIndex;
	return texIndex;
}

glm::mat4 dae::GLTFLoader::ConvertMatrix(const aiMatrix4x4& mat) const
{
	
	return glm::mat4{
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	};
}
