#include "Shader.h"

#include <fstream>
#include <stdexcept>

#include "Common.h"
#include "Context.h"

/// Shader
/////////////////////////////////////
void dae::Shader::Destroy(const Context& context)
{
	if (module != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(context.logicalDevice.device, module, nullptr);
	}
}

///ShaderLoader
/////////////////////////////////////////////////////////

void dae::ShaderLoader::Load(const Context& context, Shader& shader) const
{
	std::vector<char> code = ReadFile();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	LOG_VULKAN_ERROR(
		vkCreateShaderModule(context.logicalDevice.device, &createInfo, nullptr, &shader.module),
		"failed to create shader module!"
	);

	shader.entryPoint = m_entryPoint;
}

dae::ShaderLoader& dae::ShaderLoader::SetFilename(std::string filename)
{
	m_filename = std::move(filename);
	return *this;
}

dae::ShaderLoader& dae::ShaderLoader::SetEntryPoint(std::string entryPoint)
{
	m_entryPoint = std::move(entryPoint);
	return *this;
}

std::vector<char> dae::ShaderLoader::ReadFile() const
{
	std::ifstream file(m_filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
