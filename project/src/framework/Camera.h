#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

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

		//Save previous mouse pos as state since glfw has no relative mouse pos function
		double lastX = 0.0, lastY = 0.0;
		bool firstInput = true;


		void Initialize(float _fovAngle = 90.f, const glm::vec3& _origin = { 0.f, 0.f, 0.f }, float _nearPlane = 1.f, float _farPlane = 1000.f)
		{
			fovAngle = _fovAngle;
			fov = glm::tan(glm::radians(fovAngle) / 2.f);

			origin = _origin;

			nearPlane = _nearPlane;
			farPlane = _farPlane;
		}

		//TODO: aanpassen zodat matrices toch members zijn van Camera, niet alleen functions
		glm::mat4 CalculateViewMatrix()
		{
			glm::mat4 rotation = CalculateRotationMatrix();

			forward = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
			up = glm::vec3(rotation * glm::vec4(0, 0, 1, 0));
			right = glm::vec3(rotation * glm::vec4(1, 0, 0, 0));

			return glm::lookAt(origin, origin + forward , up);
		}

		glm::mat4 CalculateRotationMatrix() const
		{
			glm::quat yawRotation = glm::angleAxis(totalYaw, glm::vec3(0, 0, 1));
			glm::quat pitchRotation = glm::angleAxis(totalPitch, glm::vec3(1, 0, 0));

			return glm::toMat4(yawRotation * pitchRotation);
		}

		//Should be called everytime fov / aspectRation changes
		glm::mat4 CalculateProjectionMatrix(uint32_t width, uint32_t height)
		{
			projectionMatrix = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);
			projectionMatrix[1][1] *= -1;

			return projectionMatrix;
		}

		//TODO add timer for frame independent movement
		void Update(const Timer& timer, const Window& window)
		{
			auto rawWindow = window.window;
			const float deltaTime = timer.GetElapsed();

			// --- Keyboard Input --------------------------
			int forwardMove =
				(glfwGetKey(rawWindow, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(rawWindow, GLFW_KEY_UP) == GLFW_PRESS)
				- (glfwGetKey(rawWindow, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(rawWindow, GLFW_KEY_DOWN) == GLFW_PRESS);

			int rightMove =
				(glfwGetKey(rawWindow, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(rawWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
				- (glfwGetKey(rawWindow, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(rawWindow, GLFW_KEY_LEFT) == GLFW_PRESS);

			if (forwardMove || rightMove)
			{
				constexpr float keyboardMovementSpeed{ 4.f };

				origin += static_cast<float>(forwardMove) * forward * keyboardMovementSpeed * deltaTime;
				origin += static_cast<float>(rightMove) * right * keyboardMovementSpeed * deltaTime;
			}


			// --- Calculate Mouse Movement --------------------------
			double posX, posY;
			glfwGetCursorPos(rawWindow, &posX, &posY);

			if (firstInput)
			{
				lastX = posX;
				lastY = posY;
				firstInput = false;
			}

			double deltaX = posX - lastX;
			double deltaY = posY - lastY;

			lastX = posX;
			lastY = posY;

			// --- Process Mouse Input --------------------------
			constexpr float mouseMovementSpeed{ 20.f };
			constexpr float rotationSpeed{ 20.f };

			bool leftClick = glfwGetMouseButton(rawWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
			bool rightClick = glfwGetMouseButton(rawWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		
			if (leftClick && rightClick)
			{
				//Moves up & down
				origin -= static_cast<float>(deltaY) * up * mouseMovementSpeed * deltaTime;
			}
			else if (leftClick)
			{
				//Moves the camera forward & backward and rotates left & right
				origin -= static_cast<float>(deltaY) * forward * mouseMovementSpeed * deltaTime;
				totalYaw -= static_cast<float>(deltaX) * rotationSpeed * deltaTime;
			}
			else if (rightClick)
			{
				//Rotate the viewport camera
				totalPitch -= static_cast<float>(deltaY) * rotationSpeed * deltaTime;
				totalYaw -= static_cast<float>(deltaX) * rotationSpeed * deltaTime;
			}
		}
	};
}
