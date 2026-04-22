#include "Window.h"

dae::Window::Window(int width, int height, std::string title)
  :	width{ width },
	height{height},
	title{std::move(title)},
	wasResized{false}
{
	InitWindow();
}

void dae::Window::Destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool dae::Window::ShouldClose() const
{
	return(glfwWindowShouldClose(window));
}


void dae::Window::PollEvents() const
{
	glfwPollEvents();
}

void dae::Window::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
}

void dae::Window::FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->wasResized = true;
}
