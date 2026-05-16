#pragma once

#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>

namespace dae
{
	struct Context;

	struct Shader final
	{
		VkShaderModule module = VK_NULL_HANDLE;
		std::string entryPoint{};

		void Destroy(const Context& context);
	};

	class ShaderLoader final
	{
	public:
		void Load(const Context& context, Shader& shader) const;

		ShaderLoader& SetFilename(std::string filename);
		ShaderLoader& SetEntryPoint(std::string entryPoint);

	private:
		std::vector<char> ReadFile() const;

		std::string m_filename;
		std::string m_entryPoint = "main";
	};
}
