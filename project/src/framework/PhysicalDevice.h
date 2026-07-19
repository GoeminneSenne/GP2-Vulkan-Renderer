#pragma once
#include <vulkan/vulkan_core.h>

#include "DeviceFeatures.h"
#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace dae
{
	struct Instance;

	struct PhysicalDevice final
	{
		VkPhysicalDevice handle = VK_NULL_HANDLE;
		QueueFamilyIndices queueFamilyIndices;
		SwapchainSupportDetails swapchainSupport;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
	};

	class PhysicalDeviceSelector final
	{
	public:
		PhysicalDeviceSelector& SetRequestedFeatures(const DeviceFeatures& features);
		PhysicalDeviceSelector& AddDeviceExtension(const char* extensionName);
		PhysicalDeviceSelector& AddDeviceExtensions(const std::vector<const char*>& extensionNames);

		void PickPhysicalDevice(const Instance& instance, PhysicalDevice& physicalDevice) const;

	private:
		bool IsDeviceSuitable(const Instance& instance, VkPhysicalDevice device) const;
		bool CheckDeviceExtensionsSupport(VkPhysicalDevice device) const;
		template <typename T>
		static bool AreRequiredFeaturesAvailable(const T& available, const T& requested);

		DeviceFeatures requestedFeatures{};
		std::vector<const char*> deviceExtensions;
	};

	template <typename T>
	bool PhysicalDeviceSelector::AreRequiredFeaturesAvailable(const T& available, const T& requested)
	{
		//VkPhysicalDeviceFeatures has no sType and pNext so there the offset is just 0
		constexpr size_t offset = [] {
			if constexpr (std::is_same_v<T, VkPhysicalDeviceFeatures>)
				return size_t{ 0 };
			else
				return offsetof(T, pNext) + sizeof(void*);
			}();
		
		const VkBool32* availableFeatures = reinterpret_cast<const VkBool32*>(
			reinterpret_cast<const char*>(&available) + offset);

		const VkBool32* requestedFeatures = reinterpret_cast<const VkBool32*>(
			reinterpret_cast<const char*>(&requested) + offset);

		constexpr size_t featureCount = (sizeof(T) - offset) / sizeof(VkBool32);

		for (size_t idx{}; idx < featureCount; ++idx)
		{
			if (requestedFeatures[idx] && !availableFeatures[idx])
				return false;
		}

		return true;
	}
}
