#pragma once

#include <glm/glm.hpp>
#include "Image.h"
#include "Context.h"

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
		alignas(16) glm::mat4 model{};
		uint32_t albedoTexIndex{};
		uint32_t normalTexIndex{};
		uint32_t metallicRoughessTexIndex{};
	};

	// --- G Buffer -----------------------------------
	struct GBuffer
	{
		Image depthImage{};
		Image albedoImage{};
		Image normalImage{};
		Image metallicRoughnessImage{};

		void Destroy(const Context& context)
		{
			depthImage.Destroy(context);
			albedoImage.Destroy(context);
			normalImage.Destroy(context);
			metallicRoughnessImage.Destroy(context);
		}
	};

}