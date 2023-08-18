#pragma once

namespace Anker {

// Window is a thin abstraction over the platform-specific window. It takes care
// of basic interacting between the window system and the engine.
class Window {
  public:
	Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) noexcept = delete;
	Window& operator=(Window&&) noexcept = delete;
	~Window() noexcept;

	Vec2i size() const;

	bool isClosed();

	HWND nativeHandle() { return m_nativeHandle; }
	operator HWND() { return m_nativeHandle; }

	GLFWwindow* glfwWindow() { return m_glfwWindow; }
	operator GLFWwindow*() { return m_glfwWindow; }

  private:
	HWND m_nativeHandle = nullptr;
	GLFWwindow* m_glfwWindow = nullptr;
};

} // namespace Anker
