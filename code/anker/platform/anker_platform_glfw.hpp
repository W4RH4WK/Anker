#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Anker {

using NativeWindow = HWND;

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
  public:
	Vec2i windowSize();

	GLFWwindow* glfwWindow() { return m_glfwWindow; }
	NativeWindow nativeWindow() { return m_nativeWindow; }

  private:
	GLFWwindow* m_glfwWindow = nullptr;
	NativeWindow m_nativeWindow = nullptr;

	////////////////////////////////////////////////////////////
	// Input / Cursor
  public:
	Vec2 cursorPosition();

	// Hide cursor state is reset on Platform::tick. Any system that wants to
	// hide the cursor has to call this function on every tick. If no system
	// calls this function, the cursor is shown.
	void hideCursor();

  private:
	bool m_hideCursor = false;

	////////////////////////////////////////////////////////////
	// ImGui
  public:
	void imguiImplInit();
	void imguiImplNewFrame();
	void imguiImplShutdown();
};

inline std::optional<Platform> g_platform;

} // namespace Anker
