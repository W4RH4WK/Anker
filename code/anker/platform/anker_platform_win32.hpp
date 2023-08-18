#pragma once

namespace Anker {

// The Platform abstracts away various operating system specific parts, like
// window management and mouse cursor handling. It is initialized before the
// engine and can be accessed globally. Certain events (e.g. window resize) are
// forwarded to the global engine instance, given it has been initialized.
class Platform {
  public:
	Platform();
	Platform(const Platform&) = delete;
	Platform& operator=(const Platform&) = delete;
	Platform(Platform&&) noexcept = delete;
	Platform& operator=(Platform&&) noexcept = delete;
	~Platform() noexcept;

	void tick();

	bool shouldShutdown();

	////////////////////////////////////////////////////////////
	// Window

	Vec2i windowSize();

	GLFWwindow* glfwWindow() { return m_glfwWindow; }
	operator GLFWwindow*() { return m_glfwWindow; }

	HWND nativeHandle() { return m_nativeWindow; }
	operator HWND() { return m_nativeWindow; }

	////////////////////////////////////////////////////////////
	// Input / Cursor

	Vec2 cursorPosition();

	void hideCursor();

	////////////////////////////////////////////////////////////
	// ImGui

	void imguiImplInit();
	void imguiImplNewFrame();
	void imguiImplShutdown();

  private:
	GLFWwindow* m_glfwWindow = nullptr;
	HWND m_nativeWindow = nullptr;

	bool m_hideCursor = false;
};

inline std::optional<Platform> g_platform;

} // namespace Anker
