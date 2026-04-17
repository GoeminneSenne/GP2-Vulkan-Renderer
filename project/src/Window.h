#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace dae
{
	class Window
	{
	public:
		explicit Window(int width, int height, std::string title);
		~Window();

		void PollEvents() const;
		
		bool ShouldClose() const;
		bool WasResize() const;
		GLFWwindow* GetGLFWwindow() const;

		Window(const Window& other) = delete;
		Window(Window&& other) = delete;
		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) = delete;

	private:
		void InitWindow();
		static void FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);

		int m_width;
		int m_height;
		std::string m_title;
		bool m_wasResized;

		GLFWwindow* m_pWindow;
	};
}