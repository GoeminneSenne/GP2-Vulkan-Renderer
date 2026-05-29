#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "Image.h"
#include "Vertex.h"

namespace dae
{
	struct Context;

	struct Mesh final
	{
		void LoadObj(const Context& context, const std::string& path);
		void Upload(const Context& context, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		void Destroy(const Context& context);

		void RecordDraw(const CommandBuffer& command) const;

		Buffer vertexBuffer{};
		Buffer indexBuffer{};
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		uint32_t materialIndex = 0;
	};


	struct Material
	{
		Image* baseColor;
		Image* normalMap;

		glm::vec4 baseColorFactor = {1,1,1,1};
		float metallic = 1.f;
		float roughness = 1.f;
	};

	struct Node
	{
		glm::mat4 transform;
		std::vector<Mesh*> meshes;
		//std::vector<Node*> children;

		std::vector<uint32_t> meshIndices;
		std::vector<uint32_t> children;

		void RecordDraw(const CommandBuffer& command) const;
		
	};

	struct Scene
	{
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
		std::vector<Image> textures;
		std::vector<Node> nodes;

		uint32_t rootNode = 0;

		Node* root = nullptr;

		void Destroy(const Context& context);
	};
}
