#pragma once
#include <vector>

#include "PhysicalDevice.h"

namespace dae
{
	struct QueueFamilyIndices;
	struct Instance;
	struct PhysicalDevice;

	class PhysicalDeviceSelector final
	{
	public:
		PhysicalDeviceSelector& AddDeviceExtension(const char* extensionName);

		void PickPhysicalDevice(const Instance& instance, PhysicalDevice& physicalDevice) const;

	private:
		bool IsDeviceSuitable(const Instance& instance, VkPhysicalDevice device) const;
		bool CheckDeviceExtensionsSupport(VkPhysicalDevice device) const;

		std::vector<const char*> deviceExtensions;
	};
}
