#include <anker/core/anker_input_system.hpp>

#include <anker/core/anker_imgui_system.hpp>
#include <anker/platform/anker_platform.hpp>

namespace Anker {

InputSystem::InputSystem(ImguiSystem& imgui) : m_imgui(imgui) {}

void InputSystem::tick(float dt)
{
	// Suppress actions during ImGui interaction
	if (ImGui::GetIO().WantTextInput) {
		m_actions = {};
		return;
	}

	using Platform::inputValue;

	m_actions.playerMoveLeft.tick(dt, inputValue(MkbInput::Left) + inputValue(GamepadInput::LsLeft));
	m_actions.playerMoveRight.tick(dt, inputValue(MkbInput::Right) + inputValue(GamepadInput::LsRight));
	m_actions.playerMoveDown.tick(dt, inputValue(MkbInput::Down) + inputValue(GamepadInput::LsDown));
	m_actions.playerJump.tick(dt, inputValue(MkbInput::Space) + inputValue(GamepadInput::A));
	m_actions.playerDash.tick(dt, inputValue(MkbInput::LeftShift) + inputValue(GamepadInput::B));

	m_actions.editorToggle.tick(dt, inputValue(MkbInput::F1));
	m_actions.editorMapReload.tick(dt, inputValue(MkbInput::F5));
	m_actions.editorCameraActivate = inputValue(MkbInput::MouseRight);
	m_actions.editorCameraPan = {-Platform::cursorDelta().x, Platform::cursorDelta().y};
	m_actions.editorCameraZoom = Platform::scrollDelta().y;
}

} // namespace Anker
