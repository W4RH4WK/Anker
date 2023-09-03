#include <anker/editor/anker_editor_framework.hpp>

#include <anker/core/anker_engine.hpp>

namespace Anker {

void EditorFramework::tick(float dt, Scene& scene)
{
	m_inspector.tick(dt, scene);
	m_cameraSystem.tick(dt, scene);

	if (ImGui::BeginMainMenuBar()) {
		m_inspector.drawMenuBarEntry();
		m_cameraSystem.drawMenuBarEntry(scene);

		ImGui::ToggleButton("PhysDbg", &g_engine->physicsSystem.debugDraw);

		ImGui::EndMainMenuBar();
	}
}

} // namespace Anker