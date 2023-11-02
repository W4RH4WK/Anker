#include <anker/core/anker_input_system.hpp>

#include <anker/platform/anker_platform.hpp>

namespace Anker {

void InputSystem::tick(float dt)
{
	// Suppress actions during ImGui interaction
	if (ImGui::GetIO().WantTextInput) {
		m_actions = {};
		return;
	}

	for_each(refl::reflect<Actions>().members, [&](auto member) { //
		member(m_actions).tick(dt);
	});

	// Hard-coded bindings:
	m_actions.editorCameraPan = {-Platform::cursorDelta().x, Platform::cursorDelta().y};
	m_actions.editorCameraZoom = Platform::scrollDelta().y;
}

} // namespace Anker
