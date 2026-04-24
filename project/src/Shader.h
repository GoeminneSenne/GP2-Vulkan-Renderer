#pragma once

#include <vulkan/vulkan_core.h>
#include <string>

namespace dae
{
	struct Shader final
	{
		VkShaderModule module = VK_NULL_HANDLE;
		std::string entryPoint{};

		void Destroy(const Context& context)
		{
			if (module != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(context.logicalDevice.device, module, nullptr);
			}
		}
	};
}