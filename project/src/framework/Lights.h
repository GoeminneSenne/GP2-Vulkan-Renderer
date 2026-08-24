#pragma once

namespace dae
{
	struct alignas(16) DirectionalLight
	{
		glm::vec3 direction{};
		float lux{};
		glm::vec3 color{};
		float _padding{FLT_MAX};
	};

	struct alignas(16) PointLight
	{
		glm::vec3 position{};
		float lumens{};
		glm::vec3 color{};
		float _padding{ FLT_MAX };
	};
}