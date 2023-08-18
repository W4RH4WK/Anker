#include <anker/platform/anker_input_system_win32.hpp>

#include <anker/core/anker_imgui_system.hpp>
#include <anker/platform/anker_window_win32.hpp>

namespace Anker {

static Vec2 cursorPosition(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return {float(x), float(y)};
}

InputSystem::InputSystem(Window& window, ImguiSystem& imgui) : m_window(window), m_imgui(imgui)
{
	ANKER_CHECK(glfwRawMouseMotionSupported());
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, true);
}

void InputSystem::tick(float)
{
	// Any component that wants to hide the cursor calls hideCursor. If none
	// does, we display the cursor as normal.
	glfwSetInputMode(m_window, GLFW_CURSOR, m_hideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	m_hideCursor = false;

	// Track relative cursor movement
	Vec2 cursor = cursorPosition(m_window);
	const Vec2 cursorDelta = cursor - m_previousCursorPosition;
	m_previousCursorPosition = cursor;

	// Suppress actions during ImGui interaction
	if (ImGui::GetIO().WantTextInput) {
		m_actions = {};
		return;
	}

	// auto pressed = [&](int key) { return glfwGetKey(m_window, key) == GLFW_PRESS; };
	auto mousePressed = [&](int button) { return glfwGetMouseButton(m_window, button) == GLFW_PRESS; };

	m_actions = {
	    .editorCameraActivate = mousePressed(GLFW_MOUSE_BUTTON_2),
	    .editorCameraPan = {-cursorDelta.x, cursorDelta.y},
	    .editorCameraZoom = m_scrollDelta,
	};

	m_scrollDelta = 0;
}

void InputSystem::hideCursor()
{
	m_hideCursor = true;
}

void InputSystem::onScroll(float yoffset)
{
	m_scrollDelta += yoffset;
}

} // namespace Anker
