#include "Window.h"

dae::Window::Window(int width, int height, std::string title)
  :	m_width{ width },
	m_height{height},
	m_title{std::move(title)},
	m_wasResized{false}
{
	InitWindow();
}

dae::Window::~Window()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

bool dae::Window::ShouldClose() const
{
	return(glfwWindowShouldClose(m_pWindow));
}

bool dae::Window::WasResized()
{
	bool value = m_wasResized;
	m_wasResized = false;
	return value;
}

GLFWwindow* dae::Window::GetGLFWwindow() const
{
	return m_pWindow;
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

	m_pWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_pWindow, this);
	glfwSetFramebufferSizeCallback(m_pWindow, FramebufferResizeCallback);
}

void dae::Window::FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_wasResized = true;
}
