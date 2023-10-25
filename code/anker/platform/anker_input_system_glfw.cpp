#include <anker/platform/anker_input_system_glfw.hpp>

#include <anker/core/anker_imgui_system.hpp>
#include <anker/platform/anker_platform_glfw.hpp>

namespace Anker {

InputSystem::InputSystem(ImguiSystem& imgui) : m_imgui(imgui) {}

void InputSystem::tick(float dt)
{
	// Suppress actions during ImGui interaction
	if (ImGui::GetIO().WantTextInput) {
		m_actions = {};
		return;
	}

	const auto& inputState = Platform::inputState();

	m_actions.playerMoveLeft.tick(dt, inputState[KeyInput::Left] + inputState[GamepadInput::LsLeft]);
	m_actions.playerMoveRight.tick(dt, inputState[KeyInput::Right] + inputState[GamepadInput::LsRight]);
	m_actions.playerMoveDown.tick(dt, inputState[KeyInput::Down] + inputState[GamepadInput::LsDown]);
	m_actions.playerJump.tick(dt, inputState[KeyInput::Space] + inputState[GamepadInput::A]);
	m_actions.playerDash.tick(dt, inputState[KeyInput::Shift] + inputState[GamepadInput::B]);

	auto* window = Platform::glfwWindow();
	auto pressed = [&](int key) { return glfwGetKey(window, key) == GLFW_PRESS; };
	auto mousePressed = [&](int button) { return glfwGetMouseButton(window, button) == GLFW_PRESS; };

	m_actions.editorToggle.tick(dt, pressed(GLFW_KEY_F1));
	m_actions.editorMapReload.tick(dt, pressed(GLFW_KEY_F5));
	m_actions.editorCameraActivate = mousePressed(GLFW_MOUSE_BUTTON_2);
	m_actions.editorCameraPan = {-inputState.cursorDelta.x, inputState.cursorDelta.y};
	m_actions.editorCameraZoom = inputState.scrollDelta;
}

} // namespace Anker
