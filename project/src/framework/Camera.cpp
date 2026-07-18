#include "Camera.h"

void dae::Camera::Initialize(float _fovAngle, const glm::vec3& _origin, float _nearPlane, float _farPlane)
{
	fovAngle = _fovAngle;
	fov = glm::tan(glm::radians(fovAngle) / 2.f);

	origin = _origin;

	nearPlane = _nearPlane;
	farPlane = _farPlane;
}

glm::mat4 dae::Camera::CalculateViewMatrix()
{
	glm::mat4 rotation = CalculateRotationMatrix();

	forward = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
	up = glm::vec3(rotation * glm::vec4(0, 0, 1, 0));
	right = glm::vec3(rotation * glm::vec4(1, 0, 0, 0));

	viewMatrix = glm::lookAt(origin, origin + forward, up);
	return viewMatrix;
}

glm::mat4 dae::Camera::CalculateRotationMatrix() const
{
	glm::quat yawRotation = glm::angleAxis(totalYaw, glm::vec3(0, 0, 1));
	glm::quat pitchRotation = glm::angleAxis(totalPitch, glm::vec3(1, 0, 0));

	return glm::toMat4(yawRotation * pitchRotation);
}

glm::mat4 dae::Camera::CalculateProjectionMatrix(uint32_t width, uint32_t height)
{
	projectionMatrix = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), nearPlane, farPlane);
	projectionMatrix[1][1] *= -1;

	return projectionMatrix;
}

void dae::Camera::Update(const Timer& timer, const Window& window)
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

	CalculateViewMatrix();
}
