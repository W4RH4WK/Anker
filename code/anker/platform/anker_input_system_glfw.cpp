#include <anker/platform/anker_input_system_glfw.hpp>

#include <anker/core/anker_imgui_system.hpp>
#include <anker/platform/anker_platform_glfw.hpp>

namespace Anker {

InputSystem::InputSystem(ImguiSystem& imgui) : m_imgui(imgui) {}

void InputSystem::tick(float)
{
	// Track relative cursor movement
	Vec2 cursor = Platform::cursorPosition();
	const Vec2 cursorDelta = cursor - m_previousCursorPosition;
	m_previousCursorPosition = cursor;

	// Suppress actions during ImGui interaction
	if (ImGui::GetIO().WantTextInput) {
		m_actions = {};
		return;
	}

	auto* window = Platform::glfwWindow();
	// auto pressed = [&](int key) { return glfwGetKey(m_window, key) == GLFW_PRESS; };
	auto mousePressed = [&](int button) { return glfwGetMouseButton(window, button) == GLFW_PRESS; };

	m_actions = {
	    .editorCameraActivate = mousePressed(GLFW_MOUSE_BUTTON_2),
	    .editorCameraPan = {-cursorDelta.x, cursorDelta.y},
	    .editorCameraZoom = m_scrollDelta,
	};

	m_scrollDelta = 0;
}

void InputSystem::onScroll(float yoffset)
{
	m_scrollDelta += yoffset;
}

} // namespace Anker
