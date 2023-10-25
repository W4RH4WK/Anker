#include <anker/platform/anker_input_system_glfw.hpp>

#include <anker/core/anker_imgui_system.hpp>
#include <anker/platform/anker_platform_glfw.hpp>

namespace Anker {

InputSystem::InputSystem(ImguiSystem& imgui) : m_imgui(imgui) {}

void InputSystem::tick(float dt)
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
	auto pressed = [&](int key) { return glfwGetKey(window, key) == GLFW_PRESS; };
	auto mousePressed = [&](int button) { return glfwGetMouseButton(window, button) == GLFW_PRESS; };

	GLFWgamepadstate gamepadState = {};
	auto gamepadPressed = [&](int button) -> float { return gamepadState.buttons[button] == GLFW_PRESS; };
	if (Platform::windowHasFocus()) {
		glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadState);
	}

	m_actions.playerMoveLeft.tick(dt, pressed(GLFW_KEY_LEFT) + gamepadPressed(GLFW_GAMEPAD_BUTTON_DPAD_LEFT));
	m_actions.playerMoveRight.tick(dt, pressed(GLFW_KEY_RIGHT) + gamepadPressed(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT));
	m_actions.playerMoveDown.tick(dt, pressed(GLFW_KEY_DOWN) + gamepadPressed(GLFW_GAMEPAD_BUTTON_DPAD_DOWN));
	m_actions.playerJump.tick(dt, pressed(GLFW_KEY_SPACE) || gamepadPressed(GLFW_GAMEPAD_BUTTON_A));
	m_actions.playerDash.tick(dt, pressed(GLFW_KEY_LEFT_SHIFT) || gamepadPressed(GLFW_GAMEPAD_BUTTON_B));

	m_actions.editorToggle.tick(dt, pressed(GLFW_KEY_F1));
	m_actions.editorMapReload.tick(dt, pressed(GLFW_KEY_F5));
	m_actions.editorCameraActivate = mousePressed(GLFW_MOUSE_BUTTON_2);
	m_actions.editorCameraPan = {-cursorDelta.x, cursorDelta.y};
	m_actions.editorCameraZoom = m_scrollDelta;

	m_scrollDelta = 0;
}

void InputSystem::onScroll(float yoffset)
{
	m_scrollDelta += yoffset;
}

} // namespace Anker
