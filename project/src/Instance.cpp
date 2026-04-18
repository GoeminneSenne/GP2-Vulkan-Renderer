#include "Instance.h"

dae::Instance::~Instance()
{
	//TODO: destroy debugmessenger
	vkDestroyInstance(instance, nullptr);
}
