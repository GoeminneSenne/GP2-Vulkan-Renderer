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

	// --- Camera Data --------------------------------
	struct CameraDataUBO
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::vec4 camPos;
		alignas(8) glm::ivec2 resolution;
		alignas(8) glm::vec2 _padding;
	};

	// --- Push Constants -----------------------------
	struct ModelPC
	{
		alignas(16) glm::mat4 model{};
		uint32_t albedoTexIndex{};
		uint32_t normalTexIndex{};
		uint32_t metallicRoughessTexIndex{};
	};

	struct LightsPC
	{
		uint32_t pointLightCount{};
		uint32_t dirLightCount{};
	};

	struct ExposurePC
	{
		float aperture{};
		float ISO{};
		float shutterSpeed{};
	};
}
