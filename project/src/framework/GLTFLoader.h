#pragma once
#include <filesystem>
#include <assimp/scene.h>

#include "Mesh.h"
#include "Context.h"

namespace dae
{

	class GLTFLoader
	{
		public:
			Scene LoadScene(const Context& context, const std::string& path);
		
		private:
			uint32_t ProcessNode(const Context& context, aiNode* aiNode, const aiScene* scene, Scene& outScene);
			uint32_t ProcessMesh(const Context& context, aiMesh* aiMesh, const aiScene* scene, Scene& outScene);
			uint32_t ProcessMaterial(const Context& context, aiMaterial* aiMat, const aiScene* scene, Scene& outScene);
			uint32_t LoadTexture(const Context& context, const std::filesystem::path& path, Scene& outScene);

			glm::mat4 ConvertMatrix(const aiMatrix4x4& mat) const;

			std::filesystem::path m_sceneDirectory{};
			std::unordered_map<std::filesystem::path, uint32_t> m_textureLookup{};

			/*
			constexpr static glm::mat4 GLTF_CONVERSION_MATRIX{
				1,0,0,0,
				0,0,1,0,
				0,-1,0,0,
				0,0,0,1
			};
			*/
	};
}
