#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace dae
{
	struct Window final
	{
		void Destroy();

		void InitWindow(int width, int height, const std::string& title);
		void PollEvents() const;
		bool ShouldClose() const;

		int width{};
		int height{};
		std::string title{};
		bool wasResized{};
		GLFWwindow* window{};

	private:
		static void FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
	};
}