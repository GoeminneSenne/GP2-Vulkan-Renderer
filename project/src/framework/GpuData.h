#pragma once

#include <glm/glm.hpp>

namespace dae
{
	// --- Uniform Buffers -----------------------------
	struct ViewProjectionUBO
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	// --- Push Constants -----------------------------
	struct PushConstants
	{
		alignas(16) glm::mat4 model;
		uint32_t textureIndex;
	};

}