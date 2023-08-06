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

	std::optional<Vec2i> wasResized()
	{
		if (m_resized) {
			auto newSize = *m_resized;
			m_resized.reset();
			return newSize;
		} else {
			return std::nullopt;
		}
	}

  private:
	HWND m_nativeHandle = nullptr;
	GLFWwindow* m_glfwWindow = nullptr;

	std::optional<Vec2i> m_resized; // set on resize events
};

} // namespace Anker
