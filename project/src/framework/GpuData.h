#pragma once

#include <glm/glm.hpp>

namespace dae
{
	struct ViewProjectionUBO
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

}