#pragma once

namespace dae
{
	struct alignas(16) DirectionalLight
	{
		union
		{
			glm::vec4 direction_lux{ 0.f, 0.f, 0.f, 0.f };
			struct
			{
				glm::vec3 direction;
				float lux;
			};
		};

		glm::vec3 color{};
		float _padding{};
	};

	struct alignas(16) PointLight
	{
		glm::vec3 position{};
		float lumens{};
		glm::vec3 color{};
		float _padding{ FLT_MAX };
	};
}