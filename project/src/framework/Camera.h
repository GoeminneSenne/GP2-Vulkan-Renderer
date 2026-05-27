#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace dae
{
	struct Camera final
	{
		Camera() = default;
		Camera(const glm::vec3& _origin, float _fovAngle) :
			origin{_origin},
			fovAngle{_fovAngle}
		{}

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

		void Initialize(float _fovAngle = 90.f, const glm::vec3& _origin = { 0.f, 0.f, 0.f }, float _nearPlane = 1.f, float _farPlane = 1000.f)
		{
			fovAngle = _fovAngle;
			fov = glm::tan(glm::radians(fovAngle) / 2.f);

			origin = _origin;

			nearPlane = _nearPlane;
			farPlane = _farPlane;
		}

		//TODO: aanpassen zodat matrices toch members zijn van Camera, niet alleen functions
		glm::mat4 CalculateViewMatrix() const
		{
			glm::mat4 rotation = CalculateRotationMatrix();

			glm::vec3 currentForward = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
			glm::vec3 currentUp = glm::vec3(rotation * glm::vec4(0, 0, 1, 0));

			return glm::lookAt(origin, origin + currentForward , currentUp);
		}

		glm::mat4 CalculateRotationMatrix() const
		{
			glm::quat yawRotation = glm::angleAxis(totalYaw, glm::vec3(0, 0, 1));
			glm::quat pitchRotation = glm::angleAxis(totalPitch, glm::vec3(1, 0, 0));

			return glm::toMat4(yawRotation * pitchRotation);
		}

		glm::mat4 CalculateProjectionMatrix(uint32_t width, uint32_t height)
		{
			projectionMatrix = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);
			projectionMatrix[1][1] *= -1;

			return projectionMatrix;
		}

		//TODO add timer for frame independent movement
		void Update()
		{
		}
	};
}
