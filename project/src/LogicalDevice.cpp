#include "LogicalDevice.h"

dae::LogicalDevice::~LogicalDevice()
{
	vkDestroyDevice(device, nullptr);
}
