#pragma once
#include <assimp/scene.h>

#include "Mesh.h"

namespace dae
{

	class GLTFLoader
	{
		public:
			Scene LoadFile(const std::string& path);
		
		private:
			uint32_t ProcessNode(aiNode* node, const aiScene* scene, Scene& outScene);
			uint32_t ProcessMesh(aiMesh* mesh, const aiScene* scene, Scene& outScene);
			uint32_t ProcessMaterial(aiMaterial* mat, const aiScene* scene, Scene& outScene);
			uint32_t LoadTexture(aiString path, Scene& outScene);

			glm::mat4 ConvertMatrix(const aiMatrix4x4& mat);
	};
}
