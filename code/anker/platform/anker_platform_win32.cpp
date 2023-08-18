#include <anker/platform/anker_platform_win32.hpp>

#include <anker/core/anker_engine.hpp>

namespace Anker {

Platform::Platform()
{
	glfwSetErrorCallback([](int error_code, const char* description) { //
		ANKER_ERROR("GLFW: {} {}", error_code, description);
	});

	if (!glfwInit()) {
		ANKER_FATAL("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_glfwWindow = glfwCreateWindow(1280, 720, "Anker", nullptr, nullptr);
	if (!m_glfwWindow) {
		ANKER_FATAL("Could not create window");
	}

	glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow*, int width, int height) {
		if (width > 0 && height > 0 && g_engine) {
			g_engine->onResize({width, height});
		}
	});

	glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow*, double, double yoffset) {
		if (g_engine) {
			g_engine->inputSystem.onScroll(float(yoffset));
		}
	});

	m_nativeWindow = glfwGetWin32Window(m_glfwWindow);

	ANKER_CHECK(glfwRawMouseMotionSupported());
	glfwSetInputMode(m_glfwWindow, GLFW_RAW_MOUSE_MOTION, true);
}

Platform::~Platform()
{
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}

void Platform::tick()
{
	glfwPollEvents();

	// Any component that wants to hide the cursor calls hideCursor. If none
	// does, we display the cursor as normal.
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, m_hideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	m_hideCursor = false;
}

bool Platform::shouldShutdown()
{
	return glfwWindowShouldClose(m_glfwWindow);
}

Vec2i Platform::windowSize()
{
	Vec2i size;
	glfwGetWindowSize(m_glfwWindow, &size.x, &size.y);
	return size;
}

Vec2 Platform::cursorPosition()
{
	double x, y;
	glfwGetCursorPos(m_glfwWindow, &x, &y);
	return {float(x), float(y)};
}

void Platform::hideCursor()
{
	m_hideCursor = true;
}

void Platform::imguiImplInit()
{
	ImGui_ImplGlfw_InitForOther(m_glfwWindow, true);
}

void Platform::imguiImplNewFrame()
{
	ImGui_ImplGlfw_NewFrame();
}

void Platform::imguiImplShutdown()
{
	ImGui_ImplGlfw_Shutdown();
}

} // namespace Anker
