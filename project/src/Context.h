#pragma once


#include <vector>
#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>

#include "CommandBuffer.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"

namespace dae
{
	struct Context final
	{
		Instance instance{};
		PhysicalDevice physicalDevice{};
		LogicalDevice logicalDevice{};
		CommandPool commandPool{};
		VmaAllocator allocator{};
	};

	struct ContextConfig final
	{
		bool enableValidation{ false };
		std::vector<const char*> validationLayers;
		std::vector<const char*> instanceExtensions;
		std::vector<const char*> deviceExtensions;
	};

	class ContextFactory final
	{
	public:
		explicit ContextFactory(ContextConfig config);
		void Create(const Window* pWindow, Context& context);

	private:
		void CreateInstance(Context& context);
		void CreateSurface(Context& context, const Window* pWindow);
		void PickPhysicalDevice(Context& context);
		void CreateLogicalDevice(Context& context);
		void CreateCommandPool(Context& context);
		void CreateAllocator(Context& context);

		ContextConfig m_config{};
	};
}
