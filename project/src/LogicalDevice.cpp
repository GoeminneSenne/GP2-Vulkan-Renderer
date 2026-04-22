#include "LogicalDevice.h"

void dae::LogicalDevice::Destroy()
{
	vkDestroyDevice(device, nullptr);
}
