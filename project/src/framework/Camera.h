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

		//TODO add timer for frame independent movement
		void Update()
		{
		}
	};
}
