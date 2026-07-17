#include "Window.h"

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

void dae::Window::InitWindow(int width, int height, const std::string& title)
{
	this->width = width;
	this->height = height;
	this->title = title;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
}

void dae::Window::FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->width = width;
	window->height = height;
	window->wasResized = true;
}
