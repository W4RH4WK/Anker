#include "anker_window_win32.hpp"

namespace Anker {

Window::Window()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_glfwWindow = glfwCreateWindow(1280, 720, "Anker", nullptr, nullptr);
	if (!m_glfwWindow) {
		ANKER_FATAL("Could not create window");
	}

	glfwSetWindowUserPointer(m_glfwWindow, this);
	glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int width, int height) {
		if (width > 0 && height > 0) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			window->m_resized = {width, height};
		}
	});

	m_nativeHandle = glfwGetWin32Window(m_glfwWindow);
}

Window::~Window() noexcept
{
	glfwDestroyWindow(m_glfwWindow);
}

Vec2i Window::size() const
{
	Vec2i size;
	glfwGetWindowSize(m_glfwWindow, &size.x, &size.y);
	return size;
}

bool Window::isClosed()
{
	return glfwWindowShouldClose(m_glfwWindow);
}

} // namespace Anker
