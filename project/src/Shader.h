#pragma once

#include <vulkan/vulkan_core.h>
#include <string>

namespace dae
{
	//TODO: desciption dat default entrypoint main is als je geen meegeeft
	struct Shader final
	{
		VkShaderModule module = VK_NULL_HANDLE;
		std::string entryPoint{};

		void Destroy(const LogicalDevice& logicalDevice)
		{
			if (module != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(logicalDevice.device, module, nullptr);
			}
		}
	};
}