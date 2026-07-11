#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>

#include "../../inc/tiny_obj_loader.h"

#include <vector>
#include <unordered_map>

#include "Vertex.h"
#include "Context.h"
#include "GpuData.h"
#include "GraphicsPipeline.h"
#include "PipelineLayout.h"
#include "BufferHelpers.h"


void dae::Mesh::LoadObj(const Context& context, const std::string& path)
{
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};

	// --- OBJ parsing --------------------------
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector < tinyobj::material_t> materials;
		std::string err;


		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
		{
			throw std::runtime_error(err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.f, 1.f, 1.f };

				if (!uniqueVertices.contains(vertex))
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}

		uniqueVertices.clear();
	}

	Upload(context, vertices, indices);
}

void dae::Mesh::Upload(const Context& context, const std::vector<Vertex>& vertices,
	const std::vector<uint32_t>& indices)
{
	vertexCount = static_cast<uint32_t>(vertices.size());
	indexCount = static_cast<uint32_t>(indices.size());

	// --- Vertex Buffer --------------------------
	{
		VkDeviceSize verticesSize = sizeof(vertices[0]) * vertexCount;


		Buffer staging;
		BufferHelpers::CreateStaging(context, staging, verticesSize);
		BufferHelpers::UploadData(context, staging, vertices.data(), verticesSize);

		BufferHelpers::CreateVertex(context, vertexBuffer, verticesSize);

		CommandBuffer command = context.commandPool.BeginSingleTimeCommand();
		BufferHelpers::CopyBuffer(command, staging, vertexBuffer, verticesSize);
		context.commandPool.EndSingleTimeCommand(command);

		staging.Destroy(context);
	}

	// --- Index Buffer --------------------------
	{
		VkDeviceSize indicesSize = sizeof(indices[0]) * indexCount;

		Buffer staging;
		BufferHelpers::CreateStaging(context, staging, indicesSize);
		BufferHelpers::UploadData(context, staging, indices.data(), indicesSize);

		BufferHelpers::CreateIndex(context, indexBuffer, indicesSize);

		CommandBuffer command = context.commandPool.BeginSingleTimeCommand();
		BufferHelpers::CopyBuffer(command, staging, indexBuffer, indicesSize);
		context.commandPool.EndSingleTimeCommand(command);

		staging.Destroy(context);
	}
}

void dae::Mesh::Destroy(const Context& context)
{
	vertexBuffer.Destroy(context);
	indexBuffer.Destroy(context);
}


void dae::Mesh::RecordDraw(const CommandBuffer& command) const
{
	VkDeviceSize offset = 0;

	vkCmdBindVertexBuffers(command.handle, 0, 1, &vertexBuffer.handle, &offset);
	vkCmdBindIndexBuffer(command.handle, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(command.handle, indexCount, 1, 0, 0, 0);
}

void dae::Mesh::RecordDraw(const CommandBuffer& command, const GraphicsPipeline& pipeline, const glm::mat4& transform) const
{
	PushConstants modelPC{ transform, textureIndex };
	vkCmdPushConstants(command.handle, pipeline.layout->handle, VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(modelPC), &modelPC);

	RecordDraw(command);
}


void dae::Scene::RecordDraw(const CommandBuffer& command, const GraphicsPipeline& pipeline) const
{
	DrawNode(command, nodes[rootNode], pipeline, glm::mat4(1));
}

void dae::Scene::DrawNode(const CommandBuffer& command, const Node& node, const GraphicsPipeline& pipeline,
                          const glm::mat4& parentTransform) const
{
	glm::mat4 globalTransform = parentTransform * node.transform;

	for (const auto& meshIndex : node.meshIndices)
	{
		meshes[meshIndex].RecordDraw(command, pipeline, globalTransform);
	}

	for (const auto& childIndex : node.children)
	{
		DrawNode(command, nodes[childIndex], pipeline, globalTransform);
	}
}

void dae::Scene::Destroy(const Context& context)
{
	for (auto& mesh : meshes)
	{
		mesh.Destroy(context);
	}

	for (auto& texture : textures)
	{
		texture.Destroy(context);
	}
}
