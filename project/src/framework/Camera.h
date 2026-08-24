#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Window.h"
#include "Timer.h"

namespace dae
{
	struct Camera final
	{
		Camera() = default;
		Camera(const glm::vec3& _origin, float _fovAngle) :
			origin{_origin},
			fovAngle{_fovAngle}
		{}

		float aperture{1.4f};
		float ISO{1600.f};
		float shutterSpeed{ 1.f / 60.f };

		glm::vec3 origin{};
		float fovAngle{ 90.f };
		float fov{ glm::tan((glm::radians(fovAngle) / 2.f)) };

		glm::vec3 forward{ 0.f, 0.f, 1.f };
		glm::vec3 up{ 0.f, 1.f, 0.f };
		glm::vec3 right{ 1.f, 0.f, 0.f };

		float totalPitch{};
		float totalYaw{};

		float nearPlane{ 1.f };
		float farPlane{ 1000.f };

		glm::mat4 projectionMatrix{};
		glm::mat4 viewMatrix{};

		//Save previous mouse pos as state since glfw has no relative mouse pos function
		double lastX = 0.0, lastY = 0.0;
		bool firstInput = true;

		void Initialize(float _fovAngle = 90.f, const glm::vec3& _origin = { 0.f, 0.f, 0.f }, float _nearPlane = 1.f, float _farPlane = 1000.f);
		glm::mat4 CalculateViewMatrix();
		glm::mat4 CalculateRotationMatrix() const;
		//Should be called everytime fov / aspectRatio changes
		glm::mat4 CalculateProjectionMatrix(uint32_t width, uint32_t height);

		void Update(const Timer& timer, const Window& window);
	};
}
