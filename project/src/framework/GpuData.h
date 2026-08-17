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

	// --- Camera Data --------------------------------
	struct CameraDataUBO
	{
		alignas(16) glm::mat4 invView;
		alignas(16) glm::mat4 invProj;
		alignas(16) glm::vec4 camPos;
		alignas(8) glm::vec2 resolution;
		alignas(8) glm::vec2 _padding;
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