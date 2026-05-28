#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../inc/tiny_obj_loader.h"

#include <vector>
#include <unordered_map>

#include "Vertex.h"
#include "Context.h"


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

		vertexCount = static_cast<uint32_t>(vertices.size());
		indexCount = static_cast<uint32_t>(indices.size());
	}


	// --- Vertex Buffer --------------------------
	{
		VkDeviceSize verticesSize = sizeof(vertices[0]) * vertexCount;

		Buffer stagingBuffer;
		stagingBuffer.CreateStaging(context, verticesSize);
		stagingBuffer.UploadData(context, vertices.data(), verticesSize);

		vertexBuffer.CreateVertex(context, verticesSize);

		CommandBuffer command = context.commandPool.BeginSingleTimeCommand();
		vertexBuffer.RecordCopyFrom(command, stagingBuffer, verticesSize);
		context.commandPool.EndSingleTimeCommand(command);

		stagingBuffer.Destroy(context);
	}
	
	// --- Index Buffer --------------------------
	{
		VkDeviceSize indicesSize = sizeof(indices[0]) * indexCount;

		Buffer stagingBuffer;
		stagingBuffer.CreateStaging(context, indicesSize);
		stagingBuffer.UploadData(context, indices.data(), indicesSize);

		indexBuffer.CreateIndex(context, indicesSize);

		CommandBuffer command = context.commandPool.BeginSingleTimeCommand();
		indexBuffer.RecordCopyFrom(command, stagingBuffer, indicesSize);
		context.commandPool.EndSingleTimeCommand(command);

		stagingBuffer.Destroy(context);
	}
}

void dae::Mesh::Destroy(const Context& context)
{
	vertexBuffer.Destroy(context);
	indexBuffer.Destroy(context);
}

void dae::Mesh::RecordDraw(const CommandBuffer& command) const
{
	VkBuffer buffer = vertexBuffer.handle;
	VkDeviceSize offset = 0;

	vkCmdBindVertexBuffers(command.handle, 0, 1, &buffer, &offset);
	vkCmdBindIndexBuffer(command.handle, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(command.handle, indexCount, 1, 0, 0, 0);
}
